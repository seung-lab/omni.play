
#pragma once
#include "vmmlib/vmmlib.h"
using namespace vmml;

namespace om {
namespace coords {

class Chunk;
class Data;
class DataBbox;
class Screen;
class Norm;
class NormBbox;
class VolumeSystem;
class ScreenSystem;

class Global : public Vector3f {
 private:
  typedef Vector3f base_t;

 public:
  Global() : base_t() {}
  Global(base_t v) : base_t(v) {}
  Global(const float a) : base_t(a) {}
  Global(const float i, const float j, const float k) : base_t(i, j, k) {}

  Norm ToNorm(const VolumeSystem&) const;
  Data ToData(const VolumeSystem&, const int) const;
  Chunk ToChunk(const VolumeSystem&, const int) const;
  Screen ToScreen(ScreenSystem&) const;
  Vector3i WithAbsOffset(const VolumeSystem&) const;
  static Global FromOffsetCoords(Vector3i, const VolumeSystem&);
};

class GlobalBbox : public AxisAlignedBoundingBox<float> {
 private:
  typedef AxisAlignedBoundingBox<float> base_t;

 public:
  GlobalBbox() : base_t() {}
  GlobalBbox(Global min, Global max) : base_t(min, max) {}

  template <typename T>
  GlobalBbox(AxisAlignedBoundingBox<T> b)
      : base_t(Global(b.getMin().x, b.getMin().y, b.getMin().z),
               Global(b.getMax().x, b.getMax().y, b.getMax().z)) {}

  NormBbox ToNormBbox(const VolumeSystem&) const;
  DataBbox ToDataBbox(const VolumeSystem&, int) const;
  inline Global getMin() const { return base_t::getMin(); }
  inline Global getMax() const { return base_t::getMax(); }

  friend std::ostream& operator<<(std::ostream& os, const GlobalBbox& bbox) {
    os << "min:" << bbox._min << " max:" << bbox._max;
    return os;
  };
};

}  // namespace coords
}  // namespace om
