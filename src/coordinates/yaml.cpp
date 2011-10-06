#include "common/common.h"
#include "utility/yaml/baseTypes.hpp"

using namespace om::coords;

namespace YAML 
{
    
Emitter& operator<<(Emitter& e, const globalCoord& c) 
{
    e << (Vector3f)c;
    return e;
}

void operator>>(const Node& n, globalCoord& c)
{
    Vector3f &vec = c;
    n >> vec;
}

Emitter& operator<<(Emitter& e, const globalBbox& box)
{
    e << (AxisAlignedBoundingBox<float>)box;
    return e;
}

void operator>>(const Node& n, globalBbox& box)
{
    AxisAlignedBoundingBox<float>& bbox = box;
    n >> bbox;
}
    
} // namespace YAML