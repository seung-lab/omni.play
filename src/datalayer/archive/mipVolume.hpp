#pragma once

#include "yaml-cpp/yaml.h"
#include "volume/volumeTypes.h"
#include "datalayer/fs/fileNames.hpp"
#include "volume/channelFolder.h"
#include "volume/segmentationFolder.h"

namespace om {
namespace datalayer {
namespace archive {
    
template <typename VOL>
class volume {
private:
    VOL& vol_;
    
public:
    volume(VOL& vol)
    : vol_(vol)
    {}
    
    void Store(YAML::Emitter& out) const
    {
        out << YAML::Key << "id" << YAML::Value << vol_.id_;
        out << YAML::Key << "custom name" << YAML::Value << vol_.customName_;
        out << YAML::Key << "note" << YAML::Value << vol_.note_;
        
        out << YAML::Key << "coords" << YAML::Value << vol_.CoordinateSystem()();
        
        out << YAML::Key << "build state" << YAML::Value << vol_.mBuildState;
        
        const std::string type =
        OmVolumeTypeHelpers::GetTypeAsString(vol_.mVolDataType);
        out << YAML::Key << "type" << YAML::Value << type;
    }
    
    void Load(const YAML::Node& in)
    {
        in["id"] >> vol_.id_;
        in["custom name"] >> vol_.customName_;
        in["note"] >> vol_.note_;
        
        in["coords"] >> vol_.CoordinateSystem()();
        
        in["build state"] >> vol_.mBuildState;
        
        std::string volDataType;
        in["type"] >> volDataType;
        vol_.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(volDataType);
        
        vol_.LoadPath();
    }
};

}; // namespace archive
}; // namespace datalayer
}; // namespace om
