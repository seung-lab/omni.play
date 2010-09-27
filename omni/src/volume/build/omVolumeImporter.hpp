#ifndef OM_VOLUME_IMPORTER_HPP
#define OM_VOLUME_IMPORTER_HPP

#include "common/omCommon.h"
#include "datalayer/omDataPath.h"
#include "utility/omTimer.h"
#include "volume/build/omLoadImage.h"
#include "volume/build/omVolumeImporterHDF5.hpp"
#include "volume/omMipChunkCoord.h"
#include "volume/omVolumeData.hpp"
#include "volume/omVolumeTypes.hpp"

#include <QImage>

template <typename VOL>
class OmVolumeImporter {
private:
	VOL *const vol_;
	boost::shared_ptr<OmVolumeImporterHDF5<VOL> > hdf5_;

public:
	OmVolumeImporter(VOL* vol)
		: vol_(vol)
		, hdf5_(new OmVolumeImporterHDF5<VOL>(vol))
	{}

	bool Import(const OmDataPath& path)
	{
		printf("\timporting data...\n");
		fflush(stdout);

		OmTimer import_timer;
		import_timer.start();

		const bool ret = doImport(path);

		printf("done in %.2f secs\n",import_timer.s_elapsed());

		return ret;
	}

private:
	bool doImport(const OmDataPath& path)
	{
		allocateData(figureOutDataType(path));

		printf("\tdone allocating volume data for all mip levels; data type is %s\n",
			   OmVolumeTypeHelpers::GetTypeAsString(vol_->mVolDataType).c_str());

		if(areImportFilesImages()){
			return importImageStack();
		}

		return hdf5_->importHDF5(path);
	}

	bool areImportFilesImages()
	{
		return vol_->areImportFilesImages();
	}

	OmVolDataType figureOutDataType(const OmDataPath& path)
	{
		if(areImportFilesImages()){
			return figureOutDataTypeImage();
		}

		return hdf5_->figureOutDataTypeHDF5(path);
	}

	OmVolDataType figureOutDataTypeImage()
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

	bool importImageStack()
	{
		OmLoadImage<VOL> imageLoader(vol_);
		for( int i = 0; i < vol_->mSourceFilenamesAndPaths.size(); ++i){
			const QString fnp = vol_->mSourceFilenamesAndPaths[i].absoluteFilePath();
			imageLoader.processSlice(fnp, i);
		}
		return true;
	}

	/**
	 *	Allocate the image data for all mip level volumes.
	 *	Note: root level and leaf dim must already be set
	 */
	void allocateData(const OmVolDataType type)
	{
		assert(OmVolDataType::UNKNOWN != type.index());
		vol_->mVolDataType = type;

		std::map<int, Vector3i> levelsAndDims;

		for (int level = 0; level <= vol_->GetRootMipLevel(); level++) {
			levelsAndDims[level] = vol_->getDimsRoundedToNearestChunk(level);
		}

		vol_->getVolData()->create(vol_, levelsAndDims);
	}
};

#endif
