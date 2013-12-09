#pragma once

#include "common/enums.hpp"
#include "segment/types.hpp"
#include "datalayer/vectorFileDataSource.hpp"
#include "datalayer/file.h"
#include "datalayer/paths.hpp"

namespace om {
namespace segment {

class FileDataSource : public datalayer::VectorFileDataSource<size_t, Data> {
 public:
  FileDataSource(file::path userVolumeRoot)
      : VectorFileDataSource(userVolumeRoot, PageSize) {}

 protected:
  virtual file::path relativePath(const size_t& key) const override {
    return file::Paths::Seg::SegmentDataPage(key);
  }
};

class ListTypeFileDataSource : public datalayer::CompressedVectorFileDataSource<
    size_t, common::SegListType> {
 public:
  ListTypeFileDataSource(file::path userVolumeRoot)
      : CompressedVectorFileDataSource(userVolumeRoot, PageSize) {}

 protected:
  virtual file::path relativePath(const size_t& key) const override {
    return file::Paths::Seg::SegmentListTypesPage(key);
  }
};
}
}  // namespace om::segment::
