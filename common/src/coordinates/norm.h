#pragma once

#include "precomp.h"
#include "vmmlib/vmmlib.h"
#include "common/macro.hpp"

namespace om {
namespace coords {

class Data;
class Global;
class VolumeSystem;

class Norm : public vmml::Vector3f {
 private:
  typedef vmml::Vector3f base_t;

 public:
  Norm(base_t v, const VolumeSystem &volume) : base_t(v), volume_(volume) {}

  Norm(float x, float y, float z, const VolumeSystem &volume)
      : base_t(x, y, z), volume_(volume) {}

  Global ToGlobal() const;
  Data ToData(int) const;

 private:
  PROP_CONST(VolumeSystem &, volume);
};

class NormBbox : public vmml::AxisAlignedBoundingBox<float> {
 private:
  typedef AxisAlignedBoundingBox<float> base_t;
  const VolumeSystem &vol_;

 public:
  NormBbox(Norm min, Norm max);

  inline Norm getMin() const { return Norm(base_t::getMin(), vol_); }

  inline Norm getMax() const { return Norm(base_t::getMax(), vol_); }

  inline Norm getDimensions() const {
    return Norm(base_t::getDimensions(), vol_);
  }

  inline Norm getUnitDimensions() const {
    return Norm(base_t::getUnitDimensions(), vol_);
  }

  inline Norm getCenter() const { return Norm(base_t::getCenter(), vol_); }
};

}  // namespace coords
}  // namespace om

namespace std {
template <>
struct hash<om::coords::Norm> {
  typedef om::coords::Norm argument_type;
  typedef std::size_t value_type;

  value_type operator()(argument_type const &n) const {
    std::hash<float> hasher;
    std::size_t h1 = hasher(n.x);
    std::size_t h2 = hasher(n.y);
    std::size_t h3 = hasher(n.z);
    return h1 ^ (h2 << 1) ^ (h3 << 2);
  }
};
}
