#include "datalayer/omDataPath.h"
#include "utility/omTimer.h"
#include "volume/build/omLoadImage.h"
#include "volume/build/omVolumeImporter.hpp"
#include "volume/build/omVolumeImporterHDF5.hpp"
#include "volume/omVolumeData.hpp"
#include "volume/omMipChunkCoord.h"

#include <QImage>

/**
 * copy data to MIP 0
 *
 **/

template <typename VOL>
OmVolumeImporter<VOL>::OmVolumeImporter(VOL* vol)
	: vol_(vol)
	, hdf5_(new OmVolumeImporterHDF5<VOL>(vol))
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

	return hdf5_->importHDF5(path);
}


template <typename VOL>
bool OmVolumeImporter<VOL>::areImportFilesImages()
{
	return vol_->areImportFilesImages();
}

template <typename VOL>
OmVolDataType OmVolumeImporter<VOL>::figureOutDataType(OmDataPath& path)
{
	if(areImportFilesImages()){
		return figureOutDataTypeImage();
	}

	return hdf5_->figureOutDataTypeHDF5(path);
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

	hdf5_->allocateHDF5(levelsAndDims);
	allocateMemMap(levelsAndDims);
}

template <typename VOL>
void OmVolumeImporter<VOL>::allocateMemMap(const std::map<int, Vector3i> & levelsAndDims)
{
	vol_->getVolData()->create(vol_, levelsAndDims);
}
