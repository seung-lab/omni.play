#pragma once
#include "precomp.h"

#include "common/common.h"
#include "volume/omMipVolume.h"
#include "utility/omStringHelpers.h"

class OmVolumeAllocater {
 public:
  static std::vector<std::shared_ptr<QFile> > AllocateData(
      OmMipVolume* vol, const om::common::DataType type) {
    assert(om::common::DataType::UNKNOWN != type.index());
    vol->SetVolDataType(type);

    const int maxLevel = vol->Coords().RootMipLevel();

    std::vector<std::shared_ptr<QFile> > volFiles(maxLevel + 1);

    for (int level = 0; level <= maxLevel; ++level) {
      const Vector3<uint64_t> dims =
          vol->Coords().DimsRoundedToNearestChunk(level);

      volFiles[level] = createFile(vol, level, dims);
    }

    log_infos << "\tdone allocating volume for all mip levels; data type is "
              << OmVolumeTypeHelpers::GetTypeAsString(type).c_str();

    return volFiles;
  }

  static void ReAllocateDownsampledVolumes(OmMipVolume* vol) {
    const int maxLevel = vol->Coords().RootMipLevel();

    std::vector<std::shared_ptr<QFile> > volFiles(maxLevel + 1);

    for (int level = 1; level <= maxLevel; ++level) {
      const Vector3<uint64_t> dims =
          vol->Coords().DimsRoundedToNearestChunk(level);

      volFiles[level] = createFile(vol, level, dims);
    }

    log_infos << "\tdone reallocating volume for all mip levels";
  }

 private:
  static std::shared_ptr<QFile> createFile(OmMipVolume* vol, const int level,
                                           const Vector3<uint64_t>& dims) {
    const uint64_t bps = vol->GetBytesPerVoxel();
    const uint64_t size = dims.x * dims.y * dims.z * bps;

    log_infos << "mip " << level
              << ": size is: " << om::string::humanizeNum(size) << " ("
              << dims.x << "," << dims.y << "," << dims.z << ")";

    const std::string fnpStr =
        vol->VolPaths().Data(level, vol->getVolDataType()).string();
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
