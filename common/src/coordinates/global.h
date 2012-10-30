#pragma once

#include "vmmlib/vmmlib.h"
using namespace vmml;

namespace om {
namespace server {
class vector3d;
class bbox;
}
namespace coords {

class Chunk;
class data;
class screen;
class norm;
class normBbox;
class dataBbox;
class volumeSystem;
class screenSystem;

class Global : public Vector3f
{
private:
    typedef Vector3f base_t;

public:
    Global() : base_t() {}
    Global(base_t v) : base_t(v) {}
    Global(const float a) : base_t(a) {}
    Global(const float i, const float j, const float k)
        : base_t(i, j, k) {}
    Global(server::vector3d v);

    norm ToNorm(const volumeSystem*) const;
    data ToData(const volumeSystem*, const int) const;
    Chunk ToChunk(const volumeSystem*, const int) const;
    screen ToScreen(screenSystem*) const;
    Vector3i WithAbsOffset(const volumeSystem*) const;
    static Global FromOffsetCoords(Vector3i, const volumeSystem*);
    operator server::vector3d () const;
};

class GlobalBbox : public AxisAlignedBoundingBox<float>
{
private:
    typedef AxisAlignedBoundingBox<float> base_t;

public:
    GlobalBbox() : base_t() {}
    GlobalBbox(Global min, Global max) : base_t(min, max) {}

    GlobalBbox(server::bbox b)
        : base_t(Global(b.min), Global(b.max)) {
    }

    template<typename T>
    GlobalBbox(AxisAlignedBoundingBox<T> b)
        : base_t(Global(b.getMin().x,b.getMin().y,b.getMin().z),
                 Global(b.getMax().x,b.getMax().y,b.getMax().z))
    {}

    normBbox ToNormBbox(const volumeSystem*) const;
    dataBbox ToDataBbox(const volumeSystem*, int) const;
    operator server::bbox () const;
};

} // namespace coords
} // namespace om
