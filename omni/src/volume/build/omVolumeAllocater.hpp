#ifndef OM_VOLUME_ALLOCATER_HPP
#define OM_VOLUME_ALLOCATER_HPP

#include "common/omCommon.h"
#include "volume/omMipVolume.h"
#include "utility/omStringHelpers.h"
#include "datalayer/fs/omFileNames.hpp"

#include <QFile>

class OmVolumeAllocater {
public:

	static std::vector<om::shared_ptr<QFile> >
	AllocateData(OmMipVolume* vol,
				 const OmVolDataType type)
	{
		assert(OmVolDataType::UNKNOWN != type.index());
		vol->SetVolDataType(type);

		const int maxLevel = vol->Coords().GetRootMipLevel();

		std::vector<om::shared_ptr<QFile> > volFiles(maxLevel + 1);

		for (int level = 0; level <= maxLevel; ++level) {
			const Vector3<uint64_t> dims =
				vol->Coords().getDimsRoundedToNearestChunk(level);
			volFiles[level] = createFile(vol, level, dims);
		}

		printf("\tdone allocating volume for all mip levels; data type is %s\n",
			   OmVolumeTypeHelpers::GetTypeAsString(type).c_str());

		return volFiles;
	}

private:

	static om::shared_ptr<QFile>
	createFile(OmMipVolume* vol, const int level,
			   const Vector3<uint64_t>& dims)
	{
		const uint64_t bps = vol->GetBytesPerVoxel();
		const uint64_t size = dims.x * dims.y * dims.z * bps;

		std::cout << "mip " << level << ": size is: "
				  << om::string::humanizeNum(size)
				  << " (" << dims.x
				  << "," << dims.y
				  << "," << dims.z
				  << ")\n";

		const std::string fnpStr = OmFileNames::GetMemMapFileName(vol, level);
		const QString fnp = QString::fromStdString(fnpStr);
		QFile::remove(fnp);
		om::shared_ptr<QFile> file(om::make_shared<QFile>(fnp));
		file->resize(size);
		if(!file->open(QIODevice::ReadWrite)){
			throw OmIoException("could not open file "+fnpStr);
		}
		file->seek(size-1);
		file->putChar(0);
		file->flush();

		return file;
	}
};

#endif
