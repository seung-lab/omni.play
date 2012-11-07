#include "common/common.h"
#include "utility/yaml/yaml.hpp"
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
    out << Key << "dataDimensions" << Value << c.DataDimensions();
    out << Key << "dataResolution" << Value << c.Resolution();
    out << Key << "chunkDim" << Value << c.ChunkDimensions().x;
    // out << Key << "mMipLeafDim" << Value << c.mMipLeafDim;
    // out << Key << "mMipRootLevel" << Value << c.mMipRootLevel;
    out << Key << "absOffset" << Value << c.AbsOffset();
    out << EndMap;
    return out;
}

void operator>>(const Node& in, VolumeSystem& c)
{
    boost::optional<GlobalBbox> extent;
    om::yaml::Util::OptionalRead(in, "dataExtent", extent); // backwards compatibility
    if(extent) {
        c.SetDataDimensions(extent.get().getDimensions());
    } else {
        Vector3i dims;
        in["dataDimensions"] >> dims;
        c.SetDataDimensions(dims);
    }

    Vector3i resolution;
    in["dataResolution"] >> resolution;
    c.SetResolution(resolution);

    int chunkDim;
    in["chunkDim"] >> chunkDim; c.SetChunkDimensions(Vector3i(chunkDim));
    // in["mMipLeafDim"] >> c.mMipLeafDim;
    // in["mMipRootLevel"] >> c.mMipRootLevel;
    Vector3i offset;
    om::yaml::Util::OptionalRead(in, "absOffset", offset, Vector3i::ZERO);
    c.SetAbsOffset(offset);
}

} // namespace YAML
