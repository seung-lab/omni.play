#include "volume/io/omMemMappedVolume.h"
#include "chunks/omChunk.h"
#include "volume/build/omDownsampler.hpp"
#include "threads/omTaskManager.hpp"

class LoadMemMapFilesVisitor : public boost::static_visitor<> {
 public:
  template <typename T>
  void operator()(T& d) const {
    d.Load();
  }
};
void OmMemMappedVolume::loadMemMapFiles() {
  boost::apply_visitor(LoadMemMapFilesVisitor(), volData_);
}

class AllocMemMapFilesVisitor : public boost::static_visitor<> {
 public:
  AllocMemMapFilesVisitor(const std::map<int, Vector3i>& levDims)
      : levelsAndDims(levDims) {}
  template <typename T>
  void operator()(T& d) const {
    d.Create(levelsAndDims);
  }

 private:
  const std::map<int, Vector3i> levelsAndDims;
};
void OmMemMappedVolume::allocMemMapFiles(
    const std::map<int, Vector3i>& levDims) {
  boost::apply_visitor(AllocMemMapFilesVisitor(levDims), volData_);
}

class GetBytesPerVoxelVisitor : public boost::static_visitor<int> {
 public:
  template <typename T>
  int operator()(T& d) const {
    return d.GetBytesPerVoxel();
  }
};
int OmMemMappedVolume::GetBytesPerVoxel() const {
  return boost::apply_visitor(GetBytesPerVoxelVisitor(), volData_);
}

class GetChunkPtrVisitor : public boost::static_visitor<OmRawDataPtrs> {
 public:
  GetChunkPtrVisitor(const om::coords::Chunk& coord) : coord(coord) {}

  template <typename T>
  OmRawDataPtrs operator()(T& d) const {
    return d.GetChunkPtr(coord);
  }

 private:
  const om::coords::Chunk coord;
};
OmRawDataPtrs OmMemMappedVolume::getChunkPtrRaw(
    const om::coords::Chunk& coord) {
  return boost::apply_visitor(GetChunkPtrVisitor(coord), volData_);
}

class GetVolPtrVisitor : public boost::static_visitor<OmRawDataPtrs> {
 public:
  GetVolPtrVisitor(const int level) : level_(level) {}

  template <typename T>
  OmRawDataPtrs operator()(T& d) const {
    return d.GetPtr(level_);
  }

 private:
  const int level_;
};
OmRawDataPtrs OmMemMappedVolume::GetVolPtr(const int level) {
  return boost::apply_visitor(GetVolPtrVisitor(level), volData_);
}

class GetVolPtrTypeVisitor : public boost::static_visitor<OmRawDataPtrs> {
 public:
  template <typename T>
  OmRawDataPtrs operator()(T& d) const {
    return d.GetType();
  }
};
OmRawDataPtrs OmMemMappedVolume::GetVolPtrType() {
  return boost::apply_visitor(GetVolPtrTypeVisitor(), volData_);
}

class DownsampleVisitor : public boost::static_visitor<> {
 public:
  DownsampleVisitor(OmMipVolume* vol, OmVolDataSrcs& volData)
      : vol_(vol), volData_(volData) {}

  template <typename T>
  void operator()(T*) const {
    OmMemMappedVolumeImpl<T>& files =
        boost::get<OmMemMappedVolumeImpl<T> >(volData_);

    OmDownsampler<T> d(vol_, &files);
    d.DownsampleChooseOneVoxelOfNine();
  }

 private:
  OmMipVolume* vol_;
  OmVolDataSrcs& volData_;
};

void OmMemMappedVolume::downsample(OmMipVolume* vol) {
  OmRawDataPtrs dataType = GetVolPtrType();
  boost::apply_visitor(DownsampleVisitor(vol, volData_), dataType);
}
