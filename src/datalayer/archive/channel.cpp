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
    genericManager::Save(out, cm.manager_);
    out << EndMap;
    return out;
}

void operator>>(const Node& in, OmChannelManager& cm)
{
    genericManager::Load(in, cm.manager_);
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

Emitter& operator<<(Emitter& out, const OmMipVolCoords& c)
{
    out << BeginMap;
    out << Key << "dataDimensions" << Value << c.GetDataDimensions();
    out << Key << "dataResolution" << Value << c.GetResolution();
    out << Key << "chunkDim" << Value << c.chunkDim_;
    out << Key << "mMipLeafDim" << Value << c.mMipLeafDim;
    out << Key << "mMipRootLevel" << Value << c.mMipRootLevel;
    out << Key << "absOffset" << Value << c.GetAbsOffset();
    out << EndMap;
    return out;
}

void operator>>(const Node& in, OmMipVolCoords& c)
{
    boost::optional<om::globalBbox> extent;
    om::yaml::yamlUtil::OptionalRead(in, "dataExtent", extent); // backwards compatibility
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
    om::yaml::yamlUtil::OptionalRead(in, "absOffset", offset, Vector3i::ZERO);
    c.SetAbsOffset(offset);
}

} // namespace YAML
