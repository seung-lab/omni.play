#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omDataWriter.h"
#include "datalayer/omDataLayer.h"
#include "project/omProject.h"
#include "system/cache/omMipVolumeCache.h"
#include "system/omProjectData.h"
#include "utility/omImageDataIo.h"
#include "utility/omTimer.h"
#include "volume/build/omLoadImage.h"
#include "volume/build/omVolumeImporter.hpp"
#include "volume/omVolumeData.hpp"

#include <QImage>

/**
 * copy data to MIP 0
 *
 **/

template <typename VOL>
OmVolumeImporter<VOL>::OmVolumeImporter(VOL* vol)
	: vol_(vol)
{
}

template <typename VOL>
bool OmVolumeImporter<VOL>::import(OmDataPath& path)
{
	printf("\timporting data...\n");
	fflush(stdout);

	OmTimer import_timer;
	import_timer.start();

	const bool ret = doImport(path);

	import_timer.stop();
	printf("done in %.2f secs\n",import_timer.s_elapsed());

	return ret;
}

template <typename VOL>
bool OmVolumeImporter<VOL>::doImport(OmDataPath& path)
{
	allocateData(figureOutDataType(path));
	printf("\tdone allocating volume data for all mip levels; data type is %s\n",
	       OmVolumeTypeHelpers::GetTypeAsString(vol_->mVolDataType).c_str());

	if(areImportFilesImages()){
		return importImageStack();
	}

	return importHDF5(path);
}


template <typename VOL>
bool OmVolumeImporter<VOL>::areImportFilesImages()
{
	return vol_->areImportFilesImages();
}

template <typename VOL>
OmDataPath OmVolumeImporter<VOL>::getHDFsrcPath(OmDataReader * hdf5reader,
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
QString OmVolumeImporter<VOL>::getHDFfileNameAndPath()
{
	QFileInfoList& files = vol_->mSourceFilenamesAndPaths;

	//FIXME: don't assert, or check before calling me!
	if( 1 != files.size()){
		throw OmIoException("More than one hdf5 file specified");
	}

	return files.at(0).filePath();
}

template <typename VOL>
bool OmVolumeImporter<VOL>::importHDF5(OmDataPath & inpath)
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
					dataset_image_read_trim(src_path,
								    chunk->GetExtent());

				OmDataWrapperPtr data =
					dataVTK->newWrapper(dataVTK->getVTKptr()->GetScalarPointer(), NONE);

				OmProjectData::GetDataWriter()->
					dataset_write_raw_chunk_data(dst_path,
								     chunk->GetExtent(),
								     data);
			}
		}
	}

	hdf5reader->close();

	return true;
}

template <typename VOL>
OmVolDataType OmVolumeImporter<VOL>::figureOutDataType(OmDataPath& path)
{
	if(areImportFilesImages()){
		return figureOutDataTypeImage();
	}

	return figureOutDataTypeHDF5(path);
}

template <typename VOL>
OmVolDataType OmVolumeImporter<VOL>::figureOutDataTypeHDF5(OmDataPath & inpath)
{
	const OmMipChunkCoord coord(0,0,0,0);
	const DataBbox chunk_data_bbox = vol_->MipCoordToDataBbox(coord, 0);

	OmDataLayer dl;
	OmDataReader * hdf5reader = dl.getReader(getHDFfileNameAndPath(), true);
	hdf5reader->open();
	const OmDataPath src_path = getHDFsrcPath(hdf5reader, inpath);

	OmDataWrapperPtr dataVTK =
		hdf5reader->dataset_image_read_trim(src_path,
						    chunk_data_bbox);

	hdf5reader->close();

	return dataVTK->getVolDataType();
}

template <typename VOL>
OmVolDataType OmVolumeImporter<VOL>::figureOutDataTypeImage()
{
	const int depth = QImage(vol_->mSourceFilenamesAndPaths[0].absoluteFilePath()).depth();

	switch(depth){
	case 8:
		return OmVolDataType::UINT8;
	case 32:
		return OmVolDataType::UINT32;
	default:
		printf("image depth is %d; aborting...\n", depth);
		assert(0 && "don't know how to import image");
	}
}

template <typename VOL>
bool OmVolumeImporter<VOL>::importImageStack()
{
	OmLoadImage<VOL> imageLoader(this, vol_);
	for( int i = 0; i < vol_->mSourceFilenamesAndPaths.size(); ++i){
		const QString fnp = vol_->mSourceFilenamesAndPaths[i].absoluteFilePath();
		imageLoader.processSlice(fnp, i);
	}

	printf("\ndone with image import; copying to HDF5 file...\n");
	vol_->copyDataIn();

	return true;
}

/**
 *	Allocate the image data for all mip level volumes.
 *	Note: root level and leaf dim must already be set
 */
template <typename VOL>
void OmVolumeImporter<VOL>::allocateData(const OmVolDataType type)
{
	assert(OmVolDataType::UNKNOWN != type);
	vol_->mVolDataType = type;

	std::map<int, Vector3i> levelsAndDims;

	for (int level = 0; level <= vol_->GetRootMipLevel(); level++) {
		levelsAndDims[level] = vol_->getDimsRoundedToNearestChunk(level);
	}

	allocateHDF5(levelsAndDims);
	allocateMemMap(levelsAndDims);
}

template <typename VOL>
void OmVolumeImporter<VOL>::allocateHDF5(const std::map<int, Vector3i> & levelsAndDims)
{
	//	vol_->getVolData()->create(vol_, levelsAndDims);

	const Vector3i chunkdims = vol_->GetChunkDimensions();

	FOR_EACH(it, levelsAndDims){
		const int level = it->first;
		const Vector3i dims = it->second;

		OmDataPath path(vol_->MipLevelInternalDataPath(level));

		OmProjectData::GetDataWriter()->
			dataset_image_create_tree_overwrite(path,
							    dims,
							    chunkdims,
							    vol_->mVolDataType);
	}
}

template <typename VOL>
void OmVolumeImporter<VOL>::allocateMemMap(const std::map<int, Vector3i> & levelsAndDims)
{
	vol_->getVolData()->create(vol_, levelsAndDims);
}
