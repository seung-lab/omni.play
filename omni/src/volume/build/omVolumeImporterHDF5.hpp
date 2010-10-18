#ifndef OM_VOLUME_IMPORTER_HDF5_HPP
#define OM_VOLUME_IMPORTER_HDF5_HPP

#include "common/omCommon.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omIDataWriter.h"
#include "datalayer/omDataLayer.h"
#include "volume/omMipChunk.h"
#include "system/omProjectData.h"
#include "utility/omImageDataIo.h"

#include <QFileInfoList>

template <typename VOL>
class OmVolumeImporterHDF5 {
private:
	VOL *const vol_;
	const OmDataPath inpath_;
	boost::shared_ptr<QFile> mip0volFile_;

	Vector3i volSize_;
	OmIDataReader* hdf5reader_;
	OmDataPath src_path_;

public:
	OmVolumeImporterHDF5(VOL* vol, const OmDataPath& inpath)
		: vol_(vol)
		, inpath_(inpath)
	{
		hdf5reader_ = OmDataLayer::getReader(getHDFfileNameAndPath(), true);
		hdf5reader_->open();

		src_path_ = getHDFsrcPath();
		std::cout << "importHDF5: source path is: " << src_path_ << "\n";

        volSize_ = hdf5reader_->getChunkedDatasetDims(src_path_);
		std::cout << "importHDF5: source vol dims: " << volSize_ << "\n";
	}

	~OmVolumeImporterHDF5()
	{
		hdf5reader_->close();
	}

	bool Import(boost::shared_ptr<QFile> mip0volFile)
	{
		mip0volFile_ = mip0volFile;

		OmTimer timer;
		printf("copying in HDF5 data...\n");

		//for all coords
		const Vector3i leaf_mip_dims = vol_->MipLevelDimensionsInMipChunks(0);
		for (int z = 0; z < leaf_mip_dims.z; ++z) {
			for (int y = 0; y < leaf_mip_dims.y; ++y) {
				for (int x = 0; x < leaf_mip_dims.x; ++x) {

					const OmMipChunkCoord coord(0, x, y, z);
					copyIntoChunk(coord);
				}
			}
		}

		printf("HDF5 data copy done in %f secs\n", timer.s_elapsed());
		return true;
	}

	OmVolDataType DetermineDataType()
	{
		const DataBbox chunk_bbox(Vector3i(0,0,0), Vector3i(1,0,0));

		OmDataWrapperPtr data = hdf5reader_->readChunk(src_path_, chunk_bbox);

		return data->getVolDataType();
	}

private:

	OmDataPath getHDFsrcPath()
	{
        if(hdf5reader_->dataset_exists(inpath_)){
			return inpath_;
		}

		if(hdf5reader_->dataset_exists(OmDataPaths::getDefaultDatasetName())){
			return OmDataPaths::getDefaultDatasetName();
		}

		throw OmIoException("could not find HDF5 src path");
	}

	std::string getHDFfileNameAndPath()
	{
		QFileInfoList& files = vol_->mSourceFilenamesAndPaths;

		//FIXME: don't assert, or check before calling me!
		if( 1 != files.size()){
			throw OmIoException("More than one hdf5 file specified");
		}

		return files.at(0).filePath().toStdString();
	}

	template< typename T>
	OmDataWrapperPtr doResizePartialChunk(OmDataWrapperPtr data,
										  const DataBbox& chunkExtent,
										  const DataBbox& dataExtent)
	{
		OmTimer timer;
		const Vector3i dataSize = dataExtent.getUnitDimensions();

		//weirdness w/ hdf5 and/or boost::multi_arry requires flipping x/z
		//TODO: figure out!
		OmImage<T, 3> partialChunk(OmExtents
								   [dataSize.z]
								   [dataSize.y]
								   [dataSize.x],
								   data->getPtr<T>());

		const Vector3i chunkSize = chunkExtent.getUnitDimensions();
		std::cout << "\tresizing chunk from " << dataSize
				  << " to " << chunkSize << "\n";
		partialChunk.resize(chunkSize);

		printf("\t\tdone in %f secs\n", timer.s_elapsed());

		return OmDataWrapperFactory::produce(partialChunk.getMallocCopyOfData());
	}

	OmDataWrapperPtr resizePartialChunk(OmDataWrapperPtr data,
										const DataBbox& chunkExtent,
										const DataBbox& dataExtent)
	{
		switch(data->getVolDataType().index()){
		case OmVolDataType::INT8:
			return doResizePartialChunk<int8_t>(data, chunkExtent, dataExtent);
		case OmVolDataType::UINT8:
			return doResizePartialChunk<uint8_t>(data, chunkExtent, dataExtent);
		case OmVolDataType::INT32:
			return doResizePartialChunk<int32_t>(data, chunkExtent, dataExtent);
		case OmVolDataType::UINT32:
			return doResizePartialChunk<uint32_t>(data, chunkExtent, dataExtent);
		case OmVolDataType::FLOAT:
			return doResizePartialChunk<float>(data, chunkExtent, dataExtent);
		case OmVolDataType::UNKNOWN:
		default:
			throw OmIoException("unknown data type");
		}
	}

	void copyIntoChunk(const OmMipChunkCoord& coord)
	{
		OmDataWrapperPtr data = getChunkData(coord);

		const uint64_t chunkOffset = vol_->ComputeChunkPtrOffset(coord);
		mip0volFile_->seek(chunkOffset);
		mip0volFile_->write(static_cast<const char*>(data->getVoidPtr()),
							128*128*128*vol_->GetBytesPerSample());
	}

	OmDataWrapperPtr getChunkData(const OmMipChunkCoord& coord)
	{
		//get chunk data bbox
		OmMipChunkPtr chunk;
		vol_->GetChunk(chunk, coord);

		const DataBbox& chunkExtent = chunk->GetExtent();

        const DataBbox volExtent =
			DataBbox(Vector3i::ZERO, volSize_.x, volSize_.y, volSize_.z);

        //if data extent contains given extent, just read from data
        if (volExtent.contains(chunkExtent)) {
			return hdf5reader_->readChunk(src_path_, chunkExtent);
        }

        //intersect with given extent
        DataBbox intersect_extent = chunkExtent;
        intersect_extent.intersect(volExtent);

        if (intersect_extent.isEmpty()) {
			throw OmIoException("should not have happened");
        }

        OmDataWrapperPtr partialChunk = hdf5reader_->readChunk(src_path_,
															  intersect_extent);

		return resizePartialChunk(partialChunk,
								  chunkExtent,
								  intersect_extent);
	}
};

#endif
