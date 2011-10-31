#pragma once

#include "vmmlib/vmmlib.h"
using namespace vmml;

class OmView2dState;

namespace om {
namespace common {
class vector3d;
class bbox;
}
namespace coords {

class chunkCoord;
class dataCoord;
class screenCoord;
class normCoord;
class normBbox;
class dataBbox;
class volumeSystem;
class screenSystem;

class globalCoord : public Vector3f
{
private:
    typedef Vector3f base_t;

public:
    globalCoord() : base_t() {}
    globalCoord(base_t v) : base_t(v) {}
    globalCoord(const float a) : base_t(a) {}
    globalCoord(const float i, const float j, const float k)
        : base_t(i, j, k) {}
    globalCoord(common::vector3d v);

    normCoord toNormCoord(const volumeSystem*) const;
    dataCoord toDataCoord(const volumeSystem*, const int) const;
    chunkCoord toChunkCoord(const volumeSystem*, const int) const;
    screenCoord toScreenCoord(screenSystem*) const;
    Vector3i withAbsOffset(const volumeSystem*) const;
    static globalCoord fromOffsetCoords(Vector3i, const volumeSystem*);
    operator common::vector3d () const;
};

class globalBbox : public AxisAlignedBoundingBox<float>
{
private:
    typedef AxisAlignedBoundingBox<float> base_t;

public:
    globalBbox() : base_t() {}
    globalBbox(globalCoord min, globalCoord max) : base_t(min, max) {}
    globalBbox(common::bbox b);

    normBbox toNormBbox(const volumeSystem*) const;
    dataBbox toDataBbox(const volumeSystem*, int) const;
    operator common::bbox () const;
};

} // namespace coords
} // namespace om
