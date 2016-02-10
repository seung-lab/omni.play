#pragma once

#include "common/common.h"

#include "vmmlib/vmmlib.h"
using namespace vmml;

#include "common/macro.hpp"

namespace om {
namespace coords {

class Global;
class Norm;
class Chunk;
class GlobalBbox;
class NormBbox;
class VolumeSystem;

class Data : public vmml::Vector3i {
 private:
  typedef Vector3i base_t;

 public:
  Data(base_t v, const VolumeSystem& vol, int mipLevel);
  Data(int x, int y, int z, const VolumeSystem& vol, int mipLevel);
  Data(const Data& other)
      : base_t(other), volume_(other.volume_), mipLevel_(other.mipLevel_) {}

  friend void swap(Data& a, Data& b) {
    using std::swap;
    swap((base_t&)a, (base_t&)b);
    swap(a.volume_, b.volume_);
    swap(a.mipLevel_, b.mipLevel_);
  }

  Data& operator=(Data other) {
    swap(*this, other);
    return *this;
  }

  Global ToGlobal() const;
  Norm ToNorm() const;
  Chunk ToChunk() const;
  Vector3i ToChunkVec() const;
  int ToChunkOffset() const;
  int ToTileOffset(om::common::ViewType) const;
  int ToTileDepth(om::common::ViewType) const;
  bool IsInVolume() const;
  Data AtDifferentLevel(int) const;

  inline Data operator+(const Vector3i b) const {
    return Data(base_t::operator+(b), *volume_, mipLevel_);
  }

  inline Data operator-(const Vector3i b) const {
    return Data(base_t::operator-(b), *volume_, mipLevel_);
  }

  inline Data operator*(const int scalar) const {
    return Data(base_t::operator*(scalar), *volume_, mipLevel_);
  }

  inline Data operator/(const int scalar) const {
    return Data(base_t::operator/(scalar), *volume_, mipLevel_);
  }

  inline Data operator/(const Vector3i b) const {
    return Data(base_t::operator/(b), *volume_, mipLevel_);
  }

  inline bool operator==(const Data& other) const {
    return base_t::operator==(other) && volume_ == other.volume_ &&
           mipLevel_ == other.mipLevel_;
  }

  const VolumeSystem& volume() const { return *volume_; }

 private:
  // Stored as a pointer to allow assignment and copying.
  const VolumeSystem* volume_;
  PROP(int, mipLevel);
};

class DataBbox : public vmml::AxisAlignedBoundingBox<int> {
 private:
  typedef AxisAlignedBoundingBox<int> base_t;

 public:
  DataBbox(Data min, Data max);
  DataBbox(const VolumeSystem& vol, int level);
  DataBbox(base_t bbox, const VolumeSystem& vol, int level);

  inline Data getMin() const {
    return Data(base_t::getMin(), *volume_, mipLevel_);
  }

  inline Data getMax() const {
    return Data(base_t::getMax(), *volume_, mipLevel_);
  }

  inline Data getDimensions() const {
    return Data(base_t::getDimensions(), *volume_, mipLevel_);
  }

  inline Data getUnitDimensions() const {
    return Data(base_t::getUnitDimensions(), *volume_, mipLevel_);
  }

  inline Data getCenter() const {
    return Data(base_t::getCenter(), *volume_, mipLevel_);
  }

  inline void merge(const DataBbox& other) { base_t::merge(other); }
  inline void merge(const base_t& other) { base_t::merge(other); }
  inline void intersect(const DataBbox& other) { base_t::intersect(other); }
  inline void intersect(const base_t& other) { base_t::intersect(other); }

  GlobalBbox ToGlobalBbox() const;
  NormBbox ToNormBbox() const;
  NormBbox ToNormBboxExclusive() const;

  DataBbox AtDifferentLevel(int) const;

  inline DataBbox& operator=(const DataBbox& other) {
    base_t::operator=(other);
    return *this;
  }

  inline bool operator==(const DataBbox& other) const {
    return base_t::operator==(other) && volume_ == other.volume_ &&
           mipLevel_ == other.mipLevel_;
  }

  const VolumeSystem& volume() const { return *volume_; }

 private:
  // Stored as a pointer to allow assignment and copying.
  const VolumeSystem* volume_;
  PROP(int, mipLevel);
};

}  // namespace coords
}  // namespace om
