#include "yaml-cpp/yaml.h"
#include "project/details/omChannelManager.h"
#include "genericManager.hpp"
#include "mipVolume.hpp"
#include "datalayer/archive/baseTypes.hpp"
#include "datalayer/archive/filter.h"

namespace om {
namespace data {
namespace archive {
    
YAML::Emitter &operator<<(YAML::Emitter& out, const OmChannelManager& cm)
{
    out << YAML::BeginMap;
    genericManager::Save(out, cm.manager_);
    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, OmChannelManager& cm)
{
    genericManager::Load(in, cm.manager_);
}

YAML::Emitter& operator<<(YAML::Emitter& out, const OmChannel& chan)
{
    out << YAML::BeginMap;
    mipVolume<const OmChannel> volArchive(chan);
    volArchive.Store(out);
    
    out << YAML::Key << "Filters" << YAML::Value << chan.filterManager_;
    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, OmChannel& chan)
{
    mipVolume<OmChannel> volArchive(chan);
    volArchive.Load(in);
    
    in["Filters"] >> chan.filterManager_;
    chan.LoadVolDataIfFoldersExist();
}

YAML::Emitter& operator<<(YAML::Emitter& out, const OmMipVolCoords& c)
{
    out << YAML::BeginMap;
    out << YAML::Key << "normToDataMat" << YAML::Value << c.normToDataMat_;
    out << YAML::Key << "normToDataInvMat" << YAML::Value << c.normToDataInvMat_;
    out << YAML::Key << "dataExtent" << YAML::Value << c.dataExtent_;
    out << YAML::Key << "dataResolution" << YAML::Value << c.dataResolution_;
    out << YAML::Key << "chunkDim" << YAML::Value << c.chunkDim_;
    out << YAML::Key << "unitString" << YAML::Value << c.unitString_;
    out << YAML::Key << "dataStretchValues" << YAML::Value << c.dataStretchValues_;
    out << YAML::Key << "mMipLeafDim" << YAML::Value << c.mMipLeafDim;
    out << YAML::Key << "mMipRootLevel" << YAML::Value << c.mMipRootLevel;
    out << YAML::Key << "absOffset" << YAML::Value << c.absOffset_;
    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, OmMipVolCoords& c)
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
    in["absOffset"] >> c.absOffset_;
}

}; // namespace archive
}; // namespace data
}; // namespace om
