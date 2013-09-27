#include "common/common.h"
#include "utility/yaml/omBaseTypes.hpp"
#include "coordinates/globalCoord.h"

namespace YAML
{

Emitter& operator<<(Emitter& e, const om::globalCoord& c)
{
    e << (Vector3f)c;
    return e;
}

void operator>>(const Node& n, om::globalCoord& c)
{
    Vector3f &vec = c;
    n >> vec;
}

Emitter& operator<<(Emitter& e, const om::globalBbox& box)
{
    e << (AxisAlignedBoundingBox<float>)box;
    return e;
}

void operator>>(const Node& n, om::globalBbox& box)
{
    AxisAlignedBoundingBox<float>& bbox = box;
    n >> bbox;
}

} // namespace YAML
