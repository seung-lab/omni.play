#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omDataWriter.h"
#include "datalayer/omDataLayer.h"
#include "volume/build/omVolumeImporterHDF5.hpp"
#include "volume/omMipChunk.h"
#include "system/omProjectData.h"
#include "utility/omImageDataIo.h"

#include <QFileInfoList>

template <typename VOL>
OmVolumeImporterHDF5<VOL>::OmVolumeImporterHDF5(VOL* vol)
	: vol_(vol)
{
}

template <typename VOL>
OmDataPath OmVolumeImporterHDF5<VOL>::getHDFsrcPath(OmDataReader * hdf5reader,
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

template <typename VOL>
QString OmVolumeImporterHDF5<VOL>::getHDFfileNameAndPath()
{
	QFileInfoList& files = vol_->mSourceFilenamesAndPaths;

	//FIXME: don't assert, or check before calling me!
	if( 1 != files.size()){
		throw OmIoException("More than one hdf5 file specified");
	}

	return files.at(0).filePath();
}

template <typename VOL>
bool OmVolumeImporterHDF5<VOL>::importHDF5(OmDataPath & inpath)
{
	OmDataLayer dl;
	OmDataReader * hdf5reader = dl.getReader(getHDFfileNameAndPath(), true);
	hdf5reader->open();

	const Vector3i leaf_mip_dims = vol_->MipLevelDimensionsInMipChunks(0);
	const OmDataPath dst_path(vol_->MipLevelInternalDataPath(0));
	const OmDataPath src_path = getHDFsrcPath(hdf5reader, inpath);
	std::cout << "importHDF5: source path is: \""
		  << src_path.getString() << "\"\n";

	//for all coords
	for (int z = 0; z < leaf_mip_dims.z; ++z) {
		for (int y = 0; y < leaf_mip_dims.y; ++y) {
			for (int x = 0; x < leaf_mip_dims.x; ++x) {

				//get chunk data bbox
				const OmMipChunkCoord coord(0, x, y, z);
				OmMipChunkPtr chunk;
				vol_->GetChunk(chunk, coord);

				OmDataWrapperPtr dataVTK =
					hdf5reader->
					readChunkNotOnBoundary(src_path,
								chunk->GetExtent());

				OmDataWrapperPtr data =
					dataVTK->newWrapper(dataVTK->getVTKptr()->GetScalarPointer(), NONE);

				OmProjectData::GetDataWriter()->
					writeChunk(dst_path,
								     chunk->GetExtent(),
								     data);
			}
		}
	}

	hdf5reader->close();

	return true;
}

template <typename VOL>
OmVolDataType OmVolumeImporterHDF5<VOL>::figureOutDataTypeHDF5(OmDataPath & inpath)
{
	const OmMipChunkCoord coord(0,0,0,0);
	const DataBbox chunk_data_bbox = vol_->MipCoordToDataBbox(coord, 0);

	OmDataLayer dl;
	OmDataReader * hdf5reader = dl.getReader(getHDFfileNameAndPath(), true);
	hdf5reader->open();
	const OmDataPath src_path = getHDFsrcPath(hdf5reader, inpath);

	OmDataWrapperPtr dataVTK =
		hdf5reader->readChunkNotOnBoundary(src_path,
						    chunk_data_bbox);

	hdf5reader->close();

	return dataVTK->getVolDataType();
}

template <typename VOL>
void OmVolumeImporterHDF5<VOL>::allocateHDF5(const std::map<int, Vector3i> & levelsAndDims)
{
	//	vol_->getVolData()->create(vol_, levelsAndDims);

	const Vector3i chunkdims = vol_->GetChunkDimensions();

	FOR_EACH(it, levelsAndDims){
		const int level = it->first;
		const Vector3i dims = it->second;

		OmDataPath path(vol_->MipLevelInternalDataPath(level));

		OmProjectData::GetDataWriter()->
			allocateChunkedDataset(path,
							    dims,
							    chunkdims,
							    vol_->getVolDataType());
	}
}
