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
#include "volume/build/omVolumeImporterImageStack.hpp"

template <typename VOL>
class OmVolumeImporter {
private:
	VOL *const vol_;
	const OmDataPath path_;

public:
	OmVolumeImporter(VOL* vol, const OmDataPath& path)
		: vol_(vol)
		, path_(path)
	{}

	bool Import()
	{
		OmTimer timer;

		printf("\timporting data...\n");
		fflush(stdout);

		const bool ret = doImport();
		printf("done in %.2f secs\n", timer.s_elapsed());

		return ret;
	}

private:
	bool doImport()
	{
		if(areImportFilesImages()){
			return doImportImageStack();
		}

		return doImportHDF5();
	}

	bool doImportHDF5()
	{
		OmVolumeImporterHDF5<VOL> hdf5(vol_, path_);
		allocateData(hdf5.DetermineDataType());
		return hdf5.Import();
	}

	bool doImportImageStack()
	{
		OmVolumeImporterImageStack<VOL> images(vol_);
		allocateData(images.DetermineDataType());
		return images.Import();
	}

	bool areImportFilesImages()
	{
		return vol_->areImportFilesImages();
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

		printf("\tdone allocating volume for all mip levels; data type is %s\n",
			   OmVolumeTypeHelpers::GetTypeAsString(type).c_str());
	}
};

#endif
