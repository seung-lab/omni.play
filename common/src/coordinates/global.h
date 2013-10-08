#pragma once

#include "vmmlib/vmmlib.h"
using namespace vmml;

namespace om {
namespace server {
class vector3d;
class bbox;
}
namespace coords {

class chunk;
class data;
class screen;
class norm;
class normBbox;
class dataBbox;
class volumeSystem;
class screenSystem;

class global : public Vector3f {
 private:
  typedef Vector3f base_t;

 public:
  global() : base_t() {}
  global(base_t v) : base_t(v) {}
  global(const float a) : base_t(a) {}
  global(const float i, const float j, const float k) : base_t(i, j, k) {}
  global(server::vector3d v);

  norm toNorm(const volumeSystem*) const;
  data toData(const volumeSystem*, const int) const;
  chunk toChunk(const volumeSystem*, const int) const;
  screen toScreen(screenSystem*) const;
  Vector3i withAbsOffset(const volumeSystem*) const;
  static global fromOffsetCoords(Vector3i, const volumeSystem*);
  operator server::vector3d() const;
};

class globalBbox : public AxisAlignedBoundingBox<float> {
 private:
  typedef AxisAlignedBoundingBox<float> base_t;

 public:
  globalBbox() : base_t() {}
  globalBbox(global min, global max) : base_t(min, max) {}

  globalBbox(server::bbox b) : base_t(global(b.min), global(b.max)) {}

  template <typename T>
  globalBbox(AxisAlignedBoundingBox<T> b)
      : base_t(global(b.getMin().x, b.getMin().y, b.getMin().z),
               global(b.getMax().x, b.getMax().y, b.getMax().z)) {}

  normBbox toNormBbox(const volumeSystem*) const;
  dataBbox toDataBbox(const volumeSystem*, int) const;
  operator server::bbox() const;
};

}  // namespace coords
}  // namespace om
