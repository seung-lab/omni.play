#include "yaml-cpp/yaml.h"
#include "utility/yaml/omYaml.hpp"
#include "project/details/omChannelManager.h"
#include "utility/yaml/mipVolume.hpp"
#include "utility/yaml/baseTypes.hpp"
#include "datalayer/archive/filter.h"
#include "utility/yaml/genericManager.hpp"

namespace YAML {

Emitter &operator<<(Emitter& out, const OmChannelManager& cm)
{
    out << BeginMap;
    GenericManager::Save(out, cm.manager_);
    out << EndMap;
    return out;
}

void operator>>(const Node& in, OmChannelManager& cm)
{
    GenericManager::Load(in, cm.manager_);
}

Emitter& operator<<(Emitter& out, const OmChannel& chan)
{
    out << BeginMap;
    mipVolume<const OmChannel> volArchive(chan);
    volArchive.Store(out);

    out << Key << "Filters" << Value << chan.filterManager_;
    out << EndMap;
    return out;
}

void operator>>(const Node& in, OmChannel& chan)
{
    mipVolume<OmChannel> volArchive(chan);
    volArchive.Load(in);

    in["Filters"] >> chan.filterManager_;
    chan.LoadVolDataIfFoldersExist();
}

Emitter& operator<<(Emitter& out, const om::coords::VolumeSystem& c)
{
    out << BeginMap;
    out << Key << "dataDimensions" << Value << c.DataDimensions();
    out << Key << "dataResolution" << Value << c.Resolution();
    out << Key << "chunkDim" << Value << c.chunkDim_;
    out << Key << "mMipLeafDim" << Value << c.mMipLeafDim;
    out << Key << "mMipRootLevel" << Value << c.mMipRootLevel;
    out << Key << "absOffset" << Value << c.AbsOffset();
    out << EndMap;
    return out;
}

void operator>>(const Node& in, om::coords::VolumeSystem& c)
{
    boost::optional<om::coords::GlobalBbox> extent;
    om::yaml::util::OptionalRead(in, "dataExtent", extent); // backwards compatibility
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

    in["chunkDim"] >> c.chunkDim_;
    in["mMipLeafDim"] >> c.mMipLeafDim;
    in["mMipRootLevel"] >> c.mMipRootLevel;
    Vector3i offset;
    om::yaml::util::OptionalRead(in, "absOffset", offset, Vector3i::ZERO);
    c.SetAbsOffset(offset);
}

} // namespace YAML
