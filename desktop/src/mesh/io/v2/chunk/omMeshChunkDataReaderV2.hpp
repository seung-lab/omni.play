#pragma once
#include "precomp.h"

#include "common/common.h"
#include "datalayer/fs/omFile.hpp"
#include "volume/omSegmentation.h"
#include "mesh/io/v2/chunk/omMeshChunkTypes.h"

class OmMeshChunkDataReaderV2 {
 private:
  OmSegmentation* const vol_;
  const om::coords::Chunk& coord_;
  const QString fnp_;

 public:
  OmMeshChunkDataReaderV2(OmSegmentation* seg, const om::coords::Chunk& coord,
                          const double threshold)
      : vol_(seg), coord_(coord), fnp_(filePath()) {}

  ~OmMeshChunkDataReaderV2() {}

  // no locking needed
  template <typename T>
  std::shared_ptr<T> Read(const OmMeshFilePart& entry) {
    const int64_t numBytes = entry.totalBytes;

    assert(numBytes);

    auto ret = om::mem::Malloc<T>::NumBytes(numBytes, om::mem::ZeroFill::DONT);

    char* dataCharPtr = reinterpret_cast<char*>(ret.get());

    QFile reader(fnp_);

    om::file::old::openFileRO(reader);

    if (!reader.seek(entry.offsetIntoFile)) {
      throw om::IoException("could not seek to " +
                            om::string::num(entry.offsetIntoFile));
    }

    const int64_t bytesRead = reader.read(dataCharPtr, numBytes);

    if (bytesRead != numBytes) {
      log_infos << "could not read data; numBytes is " << numBytes
                << ", but only read " << bytesRead;
      throw om::IoException("could not read fully file");
    }

    return ret;
  }

 private:
  QString filePath() { return vol_->SegPaths().MeshData(coord_).c_str(); }
};
