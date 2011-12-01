#include "common/common.h"
#include "utility/yaml/baseTypes.hpp"

using namespace om::coords;

namespace YAML
{

Emitter& operator<<(Emitter& e, const global& c)
{
    e << (Vector3f)c;
    return e;
}

void operator>>(const Node& n, global& c)
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
Emitter& operator<<(Emitter& out, const volumeSystem& c)
{
    out << BeginMap;
    // out << Key << "dataDimensions" << Value << c.GetDataDimensions();
    // out << Key << "dataResolution" << Value << c.GetResolution();
    // out << Key << "chunkDim" << Value << c.chunkDim_;
    // out << Key << "mMipLeafDim" << Value << c.mMipLeafDim;
    // out << Key << "mMipRootLevel" << Value << c.mMipRootLevel;
    // out << Key << "absOffset" << Value << c.GetAbsOffset();
    out << EndMap;
    return out;
}

void operator>>(const Node& in, volumeSystem& c)
{
    globalBbox extent;
    in["dataExtent"] >> extent;
    c.SetDataDimensions(extent.getDimensions());

    Vector3i resolution;
    in["dataResolution"] >> resolution;
    c.SetResolution(resolution);

    int chunkDim;
    in["chunkDim"] >> chunkDim;
    c.SetChunkDimension(chunkDim);

    c.UpdateRootLevel();
    Vector3i offset;
    in["absOffset"] >> offset;
    c.SetAbsOffset(offset);
}

} // namespace YAML
