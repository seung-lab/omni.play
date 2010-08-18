#include "system/cache/omMipVolumeCache.h"
#include "volume/build/omLoadImage.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"
#include "project/omProject.h"
#include "utility/omImageDataIo.h"
#include "utility/omTimer.h"
#include "volume/build/omVolumeImporter.hpp"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "system/omProjectData.h"

#include <QImage>
#include <QThreadPool>
#include <QMutexLocker>

template <typename VOL>
OmVolumeImporter<VOL>::OmVolumeImporter(VOL* vol)
	: vol(vol)
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
	return vol->areImportFilesImages();
}

template <typename VOL>
bool OmVolumeImporter<VOL>::importHDF5(OmDataPath & dataset)
{
	//dim of leaf coords
	const Vector3i leaf_mip_dims = vol->MipLevelDimensionsInMipChunks(0);

	OmDataPath leaf_volume_path;
	leaf_volume_path.setPathQstr( vol->MipLevelInternalDataPath(0) );

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
				DataBbox chunk_data_bbox = vol->MipCoordToDataBbox(chunk_coord, 0);

				//read chunk image data from source
				OmDataWrapperPtr p_img_data =
					OmImageDataIo::om_imagedata_read_hdf5(vol->mSourceFilenamesAndPaths,
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
	const int depth = QImage(vol->mSourceFilenamesAndPaths[0].absoluteFilePath()).depth();

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

	vol->AllocInternalData(figureOutDataType());

	OmLoadImage<VOL> imageLoader(this, vol);
	for( int i = 0; i < vol->mSourceFilenamesAndPaths.size(); ++i){
		const QString fnp = vol->mSourceFilenamesAndPaths[i].absoluteFilePath();
		imageLoader.processSlice(fnp, i);
	}

	printf("\ndone with image import; copying to HDF5 file...\n");
	vol->copyDataIn();

	import_timer.stop();
	printf("done in %.2f secs\n",import_timer.s_elapsed());

	return true;
}
