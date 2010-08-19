#include "system/cache/omMipVolumeCache.h"
#include "volume/build/omLoadImage.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"
#include "project/omProject.h"
#include "utility/omImageDataIo.h"
#include "utility/omTimer.h"
#include "volume/build/omVolumeImporter.hpp"
#include "volume/omVolumeData.hpp"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "system/omProjectData.h"

#include <QImage>

template <typename VOL>
OmVolumeImporter<VOL>::OmVolumeImporter(VOL* vol)
	: vol_(vol)
{
}

// copy data to MIP 0
template <typename VOL>
bool OmVolumeImporter<VOL>::import(OmDataPath & dataset)
{
	if(areImportFilesImages()){
		return importImageStack();
	}

	return importHDF5(dataset);
}

template <typename VOL>
bool OmVolumeImporter<VOL>::areImportFilesImages()
{
	return vol_->areImportFilesImages();
}

template <typename VOL>
bool OmVolumeImporter<VOL>::importHDF5(OmDataPath & dataset)
{
	//dim of leaf coords
	const Vector3i leaf_mip_dims = vol_->MipLevelDimensionsInMipChunks(0);

	OmDataPath leaf_volume_path(vol_->MipLevelInternalDataPath(0));

	printf("\timporting data...\n");
	fflush(stdout);

	OmTimer import_timer;
	import_timer.start();

	//for all coords
	for (int z = 0; z < leaf_mip_dims.z; ++z) {
		for (int y = 0; y < leaf_mip_dims.y; ++y) {
			for (int x = 0; x < leaf_mip_dims.x; ++x) {

				//get chunk data bbox
				const OmMipChunkCoord chunk_coord = OmMipChunkCoord(0, x, y, z);
				DataBbox chunk_data_bbox = vol_->MipCoordToDataBbox(chunk_coord, 0);

				//read chunk image data from source
				OmDataWrapperPtr p_img_data =
					OmImageDataIo::om_imagedata_read_hdf5(vol_->mSourceFilenamesAndPaths,
									      chunk_data_bbox,
									      dataset);
				//write to project data
				OmProjectData::GetDataWriter()->dataset_image_write_trim(leaf_volume_path,
											 &chunk_data_bbox,
											 p_img_data);
			}
		}
	}

	import_timer.stop();

	printf("done in %.6f secs\n",import_timer.s_elapsed());
	return true;
}

template <typename VOL>
OmAllowedVolumeDataTypes OmVolumeImporter<VOL>::figureOutDataType()
{
	const int depth = QImage(vol_->mSourceFilenamesAndPaths[0].absoluteFilePath()).depth();

	switch(depth){
	case 8:
		return OM_UINT8;
	case 32:
		return OM_UINT32;
	default:
		printf("image depth is %d; aborting...\n", depth);
		assert(0 && "don't know how to import image");
	}

}

template <typename VOL>
bool OmVolumeImporter<VOL>::importImageStack()
{
	printf("\timporting data...\n");
	fflush(stdout);

	//timer start
	OmTimer import_timer;
	import_timer.start();

	allocateData(figureOutDataType());

	OmLoadImage<VOL> imageLoader(this, vol_);
	for( int i = 0; i < vol_->mSourceFilenamesAndPaths.size(); ++i){
		const QString fnp = vol_->mSourceFilenamesAndPaths[i].absoluteFilePath();
		imageLoader.processSlice(fnp, i);
	}

	printf("\ndone with image import; copying to HDF5 file...\n");
	vol_->copyDataIn();

	import_timer.stop();
	printf("done in %.2f secs\n",import_timer.s_elapsed());

	return true;
}

/**
 *	Allocate the image data for all mip level volumes.
 *	Note: root level and leaf dim must already be set
 */
template <typename VOL>
void OmVolumeImporter<VOL>::allocateData(const OmAllowedVolumeDataTypes type)
{
	assert(UNKNOWN != type);
	vol_->mVolDataType = type;

	std::map<int, Vector3i> levelsAndDims;

	for (int level = 0; level <= vol_->GetRootMipLevel(); level++) {
		levelsAndDims[level] = vol_->getDimsRoundedToNearestChunk(level);
	}

	allocateHDF5(levelsAndDims);
	allocateMemMap(levelsAndDims);

	printf("done allocating volume data for all mip levels; data type is %s\n",
	       OmVolumeTypeHelpers::GetTypeAsString(vol_->mVolDataType).c_str());
}

template <typename VOL>
void OmVolumeImporter<VOL>::allocateHDF5(const std::map<int, Vector3i> & levelsAndDims)
{
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
	vol_->volData->create(vol_, levelsAndDims);
}
