#ifndef OM_VOLUME_IMPORTER_HPP
#define OM_VOLUME_IMPORTER_HPP

#include "common/omCommon.h"
#include "datalayer/omDataPath.h"
#include "utility/omTimer.h"
#include "volume/build/omLoadImage.h"
#include "volume/build/omVolumeAllocater.hpp"
#include "volume/build/omVolumeImporterHDF5.hpp"
#include "volume/build/omVolumeImporterImageStack.hpp"
#include "volume/omMipChunkCoord.h"
#include "volume/omVolumeData.hpp"
#include "volume/omVolumeTypes.hpp"

template <typename VOL>
class OmVolumeImporter {
private:
	VOL *const vol_;
	const OmDataPath path_;
	const bool importImagesAreImages_;
	std::vector<boost::shared_ptr<QFile> > volFiles_;
	const std::vector<QFileInfo>& files_;

public:
	OmVolumeImporter(VOL* vol, const OmDataPath& path,
					 const bool importImagesAreImages,
					 const std::vector<QFileInfo>& files)
		: vol_(vol)
		, path_(path)
		, importImagesAreImages_(importImagesAreImages)
		, files_(files)
	{}

	bool Import()
	{
		OmTimer timer;

		printf("\timporting data...\n");
		fflush(stdout);

		const bool ret = doImport();
		timer.PrintDone();

		return ret;
	}

private:
	bool doImport()
	{
		if(importImagesAreImages_){
			return doImportImageStack();
		}

		return doImportHDF5();
	}

	bool doImportHDF5()
	{
		OmVolumeImporterHDF5<VOL> hdf5(vol_, path_, files_);
		allocateData(hdf5.DetermineDataType());
		return hdf5.Import(volFiles_[0]);
	}

	bool doImportImageStack()
	{
		OmVolumeImporterImageStack<VOL> images(vol_, files_);
		allocateData(images.DetermineDataType());
		return images.Import(volFiles_[0]);
	}

	void allocateData(const OmVolDataType type){
		volFiles_ = OmVolumeAllocater::AllocateData(vol_, type);
	}
};

#endif
