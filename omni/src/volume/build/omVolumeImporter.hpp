#ifndef OM_VOLUME_IMPORTER_HPP
#define OM_VOLUME_IMPORTER_HPP

#include "datalayer/fs/omFileNames.hpp"
#include "common/omCommon.h"
#include "datalayer/omDataPath.h"
#include "utility/omTimer.h"
#include "volume/build/omLoadImage.h"
#include "volume/build/omVolumeImporterHDF5.hpp"
#include "volume/omMipChunkCoord.h"
#include "volume/omVolumeData.hpp"
#include "volume/omVolumeTypes.hpp"
#include "volume/build/omVolumeImporterImageStack.hpp"

//Linux only; will fix later
#include <sys/fcntl.h>

template <typename VOL>
class OmVolumeImporter {
private:
	VOL *const vol_;
	const OmDataPath path_;
	std::vector<boost::shared_ptr<QFile> > volFiles_;

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

		volFiles_.clear();
		vol_->loadVolData();

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
		return hdf5.Import(volFiles_[0]);
	}

	bool doImportImageStack()
	{
		OmVolumeImporterImageStack<VOL> images(vol_);
		allocateData(images.DetermineDataType());
		return images.Import(volFiles_[0]);
	}

	bool areImportFilesImages()
	{
		return vol_->areImportFilesImages();
	}

	void allocateData(const OmVolDataType type)
	{
		assert(OmVolDataType::UNKNOWN != type.index());
		vol_->SetVolDataType(type);

		const int maxLevel = vol_->GetRootMipLevel();
		volFiles_.resize( maxLevel + 1 );

		for (int level = 0; level <= maxLevel; ++level) {
			const Vector3<uint64_t> dims = vol_->getDimsRoundedToNearestChunk(level);
			volFiles_[level] = createFile(level, dims);
		}

		printf("\tdone allocating volume for all mip levels; data type is %s\n",
			   OmVolumeTypeHelpers::GetTypeAsString(type).c_str());
	}

	boost::shared_ptr<QFile>
	createFile(const int level, const Vector3<uint64_t>& dims)
	{
		const uint64_t bps = vol_->GetBytesPerSample();
		const uint64_t size = dims.x * dims.y * dims.z * bps;

		std::cout << "mip " << level << ": size is: "
				  << StringHelpers::commaDeliminateNum(size)
				  << " (" << dims.x
				  << "," << dims.y
				  << "," << dims.z
				  << ")\n";

		const std::string fnpStr = OmFileNames::GetMemMapFileName(vol_, level);
		const QString fnp = QString::fromStdString(fnpStr);
		QFile::remove(fnp);
		boost::shared_ptr<QFile> file(boost::make_shared<QFile>(fnp));
		file->resize(size);
		if(!file->open(QIODevice::ReadWrite)){
			throw OmIoException("could not open file "+fnpStr);
		}
		file->seek(size-1);
		file->putChar(0);
		file->flush();

		fdatasync(file->handle());
		posix_fadvise(file->handle(), 0, 0, POSIX_FADV_DONTNEED);

		return file;
	}

};

#endif
