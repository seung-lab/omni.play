#pragma once

#include "common/common.h"
#include "volume/omMipVolume.h"
#include "utility/omStringHelpers.h"
#include "datalayer/fs/omFileNames.hpp"

#include <QFile>

class OmVolumeAllocater {
 public:
  static std::vector<std::shared_ptr<QFile> > AllocateData(
      OmMipVolume* vol, const OmVolDataType type) {
    assert(OmVolDataType::UNKNOWN != type.index());
    vol->SetVolDataType(type);

    const int maxLevel = vol->Coords().GetRootMipLevel();

    std::vector<std::shared_ptr<QFile> > volFiles(maxLevel + 1);

    for (int level = 0; level <= maxLevel; ++level) {
      const Vector3<uint64_t> dims =
          vol->Coords().getDimsRoundedToNearestChunk(level);

      volFiles[level] = createFile(vol, level, dims);
    }

    log_infos << "\tdone allocating volume for all mip levels; data type is "
              << OmVolumeTypeHelpers::GetTypeAsString(type).c_str();

    vol->VolData()->load(vol);
    log_infos << "volumes memory mapped";

    return volFiles;
  }

  static void ReAllocateDownsampledVolumes(OmMipVolume* vol) {
    const int maxLevel = vol->Coords().GetRootMipLevel();

    std::vector<std::shared_ptr<QFile> > volFiles(maxLevel + 1);

    for (int level = 1; level <= maxLevel; ++level) {
      const Vector3<uint64_t> dims =
          vol->Coords().getDimsRoundedToNearestChunk(level);

      volFiles[level] = createFile(vol, level, dims);
    }
    file->seek(size - 1);
    file->putChar(0);
    file->flush();

    log_infos << "\tdone reallocating volume for all mip levels";

    vol->VolData()->load(vol);
    log_infos << "volumes memory mapped";
  }

 private:
  static std::shared_ptr<QFile> createFile(OmMipVolume* vol, const int level,
                                           const Vector3<uint64_t>& dims) {
    const uint64_t bps = vol->GetBytesPerVoxel();
    const uint64_t size = dims.x * dims.y * dims.z * bps;

    log_infos << "mip " << level
              << ": size is: " << om::string::humanizeNum(size) << " ("
              << dims.x << "," << dims.y << "," << dims.z << ")";

    const std::string fnpStr = OmFileNames::GetMemMapFileName(vol, level);
    const QString fnp = QString::fromStdString(fnpStr);
    QFile::remove(fnp);
    std::shared_ptr<QFile> file(std::make_shared<QFile>(fnp));
    file->resize(size);
    if (!file->open(QIODevice::ReadWrite)) {
      throw om::IoException("could not open file " + fnpStr);
    }
    file->seek(size - 1);
    file->putChar(0);
    file->flush();

    return file;
  }
};
