#pragma once

#include "vmmlib/vmmlib.h"
using namespace vmml;

class OmMipVolume;
class OmView2dState;

namespace om {

class chunkCoord;
class dataCoord;
class screenCoord;
class normCoord;
class normBbox;
class dataBbox;

class globalCoord : public Vector3f {
 private:
  typedef Vector3f base_t;

 public:
  globalCoord() : base_t() {}
  globalCoord(base_t v) : base_t(v) {}
  globalCoord(const float a) : base_t(a) {}
  globalCoord(const float i, const float j, const float k) : base_t(i, j, k) {}

  normCoord toNormCoord(const OmMipVolume*) const;
  dataCoord toDataCoord(const OmMipVolume*, int) const;
  chunkCoord toChunkCoord(const OmMipVolume*, int) const;
  screenCoord toScreenCoord(OmView2dState*) const;
  Vector3i withAbsOffset(const OmMipVolume*) const;
  static globalCoord fromOffsetCoords(Vector3i, const OmMipVolume*);
};

class globalBbox : public AxisAlignedBoundingBox<float> {
 private:
  typedef AxisAlignedBoundingBox<float> base_t;

 public:
  globalBbox() : base_t() {}
  globalBbox(globalCoord min, globalCoord max) : base_t(min, max) {}

  normBbox toNormBbox(const OmMipVolume*) const;
  dataBbox ToDataBbox(const OmMipVolume*, int) const;
};

}  // namespace om