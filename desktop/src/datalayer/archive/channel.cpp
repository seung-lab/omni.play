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
    out << Key << "normToDataMat" << Value << c.normToDataMat_;
    out << Key << "normToDataInvMat" << Value << c.normToDataInvMat_;
    out << Key << "dataExtent" << Value << c.dataExtent_;
    out << Key << "dataResolution" << Value << c.dataResolution_;
    out << Key << "chunkDim" << Value << c.chunkDim_;
    out << Key << "unitString" << Value << c.unitString_;
    out << Key << "dataStretchValues" << Value << c.dataStretchValues_;
    out << Key << "mMipLeafDim" << Value << c.mMipLeafDim;
    out << Key << "mMipRootLevel" << Value << c.mMipRootLevel;
    out << Key << "absOffset" << Value << c.absOffset_;
    out << EndMap;
    return out;
}

void operator>>(const Node& in, OmMipVolCoords& c)
{
    in["normToDataMat"] >> c.normToDataMat_;
    in["normToDataInvMat"] >> c.normToDataInvMat_;
    in["dataExtent"] >> c.dataExtent_;
    in["dataResolution"] >> c.dataResolution_;
    in["chunkDim"] >> c.chunkDim_;
    in["unitString"] >> c.unitString_;
    in["dataStretchValues"] >> c.dataStretchValues_;
    in["mMipLeafDim"] >> c.mMipLeafDim;
    in["mMipRootLevel"] >> c.mMipRootLevel;
    om::yaml::yamlUtil::OptionalRead(in, "absOffset", c.absOffset_, Vector3i::ZERO);
}

} // namespace YAML
