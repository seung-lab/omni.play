#pragma once

#include "vmmlib/vmmlib.h"

class OmMipVolume;

namespace om {

class dataCoord;
class globalCoord;

class normCoord : public vmml::Vector3f {
 private:
  typedef vmml::Vector3f base_t;
  const OmMipVolume *vol_;

 public:
  normCoord(base_t v, const OmMipVolume *vol) : base_t(v), vol_(vol) {}

  normCoord(float x, float y, float z, const OmMipVolume *vol)
      : base_t(x, y, z), vol_(vol) {}

  globalCoord toGlobalCoord() const;
  dataCoord toDataCoord(int) const;

  inline const OmMipVolume *volume() const { return vol_; }
};

class normBbox : public vmml::AxisAlignedBoundingBox<float> {
 private:
  typedef AxisAlignedBoundingBox<float> base_t;
  OmMipVolume const *const vol_;

 public:
  normBbox(normCoord min, normCoord max);

  inline normCoord getMin() const { return normCoord(base_t::getMin(), vol_); }

  inline normCoord getMax() const { return normCoord(base_t::getMax(), vol_); }

  inline normCoord getDimensions() const {
    return normCoord(base_t::getDimensions(), vol_);
  }

  inline normCoord getUnitDimensions() const {
    return normCoord(base_t::getUnitDimensions(), vol_);
  }

  inline normCoord getCenter() const {
    return normCoord(base_t::getCenter(), vol_);
  }
};

}  // namespace om