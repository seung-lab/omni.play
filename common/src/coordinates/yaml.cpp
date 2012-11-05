#include "common/common.h"
#include "utility/yaml/baseTypes.hpp"

using namespace om::coords;

namespace YAML
{

Emitter& operator<<(Emitter& e, const Global& c)
{
    e << (Vector3f)c;
    return e;
}

void operator>>(const Node& n, Global& c)
{
    Vector3f &vec = c;
    n >> vec;
}

Emitter& operator<<(Emitter& e, const GlobalBbox& box)
{
    e << (AxisAlignedBoundingBox<float>)box;
    return e;
}

void operator>>(const Node& n, GlobalBbox& box)
{
    AxisAlignedBoundingBox<float>& bbox = box;
    n >> bbox;
}
Emitter& operator<<(Emitter& out, const VolumeSystem& c)
{
    out << BeginMap;
    // out << Key << "dataDimensions" << Value << c.DataDimensions();
    // out << Key << "dataResolution" << Value << c.GetResolution();
    // out << Key << "chunkDim" << Value << c.chunkDim_;
    // out << Key << "mMipLeafDim" << Value << c.mMipLeafDim;
    // out << Key << "mMipRootLevel" << Value << c.mMipRootLevel;
    // out << Key << "absOffset" << Value << c.AbsOffset();
    out << EndMap;
    return out;
}

void operator>>(const Node& in, VolumeSystem& c)
{
    GlobalBbox extent;
    in["dataExtent"] >> extent;
    c.SetDataDimensions(extent.getDimensions());

    Vector3i resolution;
    in["dataResolution"] >> resolution;
    c.SetResolution(resolution);

    int chunkDim;
    in["chunkDim"] >> chunkDim;
    c.SetChunkDimensions(Vector3i(chunkDim));

    c.UpdateRootLevel();
    Vector3i offset;
    in["absOffset"] >> offset;
    c.SetAbsOffset(offset);
}

} // namespace YAML
