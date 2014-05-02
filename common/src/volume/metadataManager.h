#pragma once

#include "precomp.h"
#include "volume/metadata.hpp"
#include "coordinates/coordinates.h"
#include "datalayer/file.h"

namespace om {
namespace volume {

class MetadataDataSource;

class MetadataManager {
 public:
  MetadataManager(MetadataDataSource& mds, file::path uri);

  virtual ~MetadataManager() {}

  void New();
  void Update();
  void Save();
  void UpdateRootLevel();

  bool HasMetadata() const {
    zi::guard g(lock_);
    return (bool)meta_;
  }

  std::string& name() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return meta_->Name;
  }
  void set_name(std::string name) {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->Name = name;
  }

  utility::UUID uuid() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return meta_->UUID;
  }
  void set_uuid(utility::UUID uuid) {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->UUID = uuid;
  }

  coords::GlobalBbox bounds() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return meta_->Bounds;
  }
  void set_bounds(coords::GlobalBbox bounds) {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->Bounds = bounds;
    coordSystem_.SetBounds(bounds);
  }

  Vector3i absOffset() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return coordSystem_.AbsOffset();
  }
  void set_absOffset(Vector3i offset) {
    zi::guard g(lock_);
    throwIfInvalid();
    auto b = meta_->Bounds;
    b.setMin(offset);
    meta_->Bounds = b;
    coordSystem_.SetBounds(b);
  }

  Vector3i dimensions() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return coordSystem_.DataDimensions();
  }
  void set_dimensions(Vector3i dimensions) {
    zi::guard g(lock_);
    throwIfInvalid();
    auto b = meta_->Bounds;
    b.setMax(b.getMin() + dimensions);
    meta_->Bounds = b;
    coordSystem_.SetBounds(b);
  }

  Vector3i resolution() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return meta_->Resolution;
  }
  void set_resolution(Vector3i resolution) {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->Resolution = resolution;
    coordSystem_.SetResolution(resolution);
  }

  int chunkDim() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return meta_->ChunkDim;
  }
  void set_chunkDim(int chunkDim) {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->ChunkDim = chunkDim;
    coordSystem_.SetChunkDimensions(Vector3i(chunkDim));
  }

  int rootMipLevel() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return meta_->RootMipLevel;
  }
  void set_rootMipLevel(int rootMipLevel) {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->RootMipLevel = rootMipLevel;
  }

  common::DataType dataType() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return meta_->DataType;
  }
  void set_dataType(common::DataType dataType) {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->DataType = dataType;
  }

  common::ObjectType volumeType() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return meta_->VolumeType;
  }
  void set_volumeType(common::ObjectType volumeType) {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->VolumeType = volumeType;
  }

  uint32_t numSegments() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return meta_->NumSegments;
  }
  void set_numSegments(uint32_t numSegments) {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->NumSegments = numSegments;
  }
  uint32_t NumSegmentsInc() {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->NumSegments++;
    return meta_->NumSegments;
  }

  uint32_t maxSegments() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return meta_->MaxSegments;
  }
  void set_maxSegments(uint32_t maxSegments) {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->MaxSegments = maxSegments;
  }
  om::common::SegID MaxSegmentsInc() {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->MaxSegments++;
    return meta_->MaxSegments;
  }
  void UpdateMaxSegments(uint32_t newMax) {
    zi::guard g(lock_);
    throwIfInvalid();
    if (meta_->MaxSegments < newMax) {
      meta_->MaxSegments = newMax;
    }
  }

  uint32_t numEdges() const {
    zi::guard g(lock_);
    throwIfInvalid();
    return meta_->NumEdges;
  }
  void set_numEdges(uint32_t numEdges) {
    zi::guard g(lock_);
    throwIfInvalid();
    meta_->NumEdges = numEdges;
  }

  const coords::VolumeSystem& coordSystem() const {
    throwIfInvalid();
    return coordSystem_;
  }
  const std::string& endpoint() const { return uri_.string(); }

 private:
  void throwIfInvalid() const {
    if (!meta_) {
      throw InvalidOperationException("No Metadata Available.");
    }
  }

  MetadataDataSource& mds_;
  file::path uri_;
  std::shared_ptr<Metadata> meta_;
  coords::VolumeSystem coordSystem_;
  zi::spinlock lock_;
};
}
}  // namespace om::volume::
