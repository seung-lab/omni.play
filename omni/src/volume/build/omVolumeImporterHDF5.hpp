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

public:
	OmVolumeImporterHDF5(VOL* vol)
		: vol_(vol)
	{}

	bool importHDF5(const OmDataPath & inpath)
	{
		OmIDataReader* hdf5reader =
			OmDataLayer::getReader(getHDFfileNameAndPath(), true);
		hdf5reader->open();

		const Vector3i leaf_mip_dims = vol_->MipLevelDimensionsInMipChunks(0);
		const OmDataPath src_path = getHDFsrcPath(hdf5reader, inpath);
		std::cout << "importHDF5: source path is: \""
				  << src_path.getString() << "\"\n";

        const Vector3i volSize = hdf5reader->getChunkedDatasetDims(src_path);
		std::cout << "importHDF5: source vol dims are: "
				  << volSize << "\n";

		//for all coords
		for (int z = 0; z < leaf_mip_dims.z; ++z) {
			for (int y = 0; y < leaf_mip_dims.y; ++y) {
				for (int x = 0; x < leaf_mip_dims.x; ++x) {

					const OmMipChunkCoord coord(0, x, y, z);
					copyIntoChunk(coord, hdf5reader, src_path);
				}
			}
		}

		hdf5reader->close();

		return true;
	}

	OmVolDataType figureOutDataTypeHDF5(const OmDataPath & inpath)
	{
		const OmMipChunkCoord coord(0,0,0,0);
		const DataBbox chunk_data_bbox = vol_->MipCoordToDataBbox(coord, 0);

		OmIDataReader * hdf5reader = OmDataLayer::getReader(getHDFfileNameAndPath(), true);
		hdf5reader->open();
		const OmDataPath src_path = getHDFsrcPath(hdf5reader, inpath);

        const Vector3i volSize = hdf5reader->getChunkedDatasetDims(src_path);
//		std::cout << "importHDF5: source vol dims are: " << volSize << "\n";

		OmDataWrapperPtr data =
			hdf5reader->readChunk(src_path, chunk_data_bbox);

		hdf5reader->close();

		return data->getVolDataType();
	}

private:

	OmDataPath getHDFsrcPath(OmIDataReader * hdf5reader,
							 const OmDataPath& inpath)
	{
        if(hdf5reader->dataset_exists(inpath)){
			return inpath;
		}

		if(hdf5reader->dataset_exists(OmDataPaths::getDefaultDatasetName())){
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
		const Vector3i dataSize = dataExtent.getUnitDimensions();

		//weirdness w/ hdf5 and/or boost::multi_arry requires flipping x/z
		//TODO: figure out!
		OmImage<T, 3> partialChunk(OmExtents
								   [dataSize.z]
								   [dataSize.y]
								   [dataSize.x],
								   data->getPtr<T>());

		const Vector3i chunkSize = chunkExtent.getUnitDimensions();
//		std::cout << "resizing from " << dataSize
//				  << " to " << chunkSize << "\n";
		partialChunk.resize(chunkSize);

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

	void copyIntoChunk(const OmMipChunkCoord& coord,
				   OmIDataReader* hdf5reader,
				   const OmDataPath& src_path)
	{
		OmDataWrapperPtr data = getChunk(coord, hdf5reader, src_path);

		OmMipChunkPtr chunk;
		vol_->GetChunk(chunk, coord);
		chunk->copyInChunkData(data);
	}

	OmDataWrapperPtr getChunk(const OmMipChunkCoord& coord,
							  OmIDataReader* hdf5reader,
							  const OmDataPath& src_path)
	{
		//get chunk data bbox
		OmMipChunkPtr chunk;
		vol_->GetChunk(chunk, coord);

		const DataBbox& chunkExtent = chunk->GetExtent();

        const Vector3i volDims = hdf5reader->getChunkedDatasetDims(src_path);
        const DataBbox volExtent =
			DataBbox(Vector3i::ZERO, volDims.x, volDims.y, volDims.z);

        //if data extent contains given extent, just read from data
        if (volExtent.contains(chunkExtent)) {
			return hdf5reader->readChunk(src_path, chunkExtent);
        }

        //intersect with given extent
        DataBbox intersect_extent = chunkExtent;
        intersect_extent.intersect(volExtent);

        if (intersect_extent.isEmpty()) {
			throw OmIoException("should not have happened");
        }

        OmDataWrapperPtr partialChunk =
			hdf5reader->readChunk(src_path, intersect_extent);
		OmDataWrapperPtr paddedChunk =
			resizePartialChunk(partialChunk,
							   chunkExtent,
							   intersect_extent);

		return paddedChunk;
	}
};
#endif
