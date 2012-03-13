#pragma once

#include "yaml-cpp/yaml.h"
#include "volume/volumeTypes.h"
#include "datalayer/fs/fileNames.hpp"
#include "volume/channelFolder.h"
#include "volume/segmentationFolder.h"

namespace YAML {

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
        out << YAML::Key << "id" << YAML::Value << vol_.getID();
        out << YAML::Key << "custom name" << YAML::Value << vol_.GetCustomName();
        out << YAML::Key << "note" << YAML::Value << vol_.GetNote();

        out << YAML::Key << "coords" << YAML::Value << vol_.CoordinateSystem();

        out << YAML::Key << "build state" << YAML::Value << vol_.mBuildState;

        const std::string type =
            om::volume::typeHelpers::GetTypeAsString(vol_.mVolDataType);
        out << YAML::Key << "type" << YAML::Value << type;
    }

    void Load(const YAML::Node& in)
    {
        int id;
        in["id"] >> id;
        vol_.setId(id);

        std::string name;
        in["custom name"] >> name;
        vol_.SetCustomName(name);

        std::string note;
        in["note"] >> note;
        vol_.SetNote(note);

        in["coords"] >> vol_.CoordinateSystem();

        in["build state"] >> vol_.mBuildState;

        std::string volDataType;
        in["type"] >> volDataType;
        vol_.mVolDataType = om::volume::typeHelpers::GetTypeFromString(volDataType);

        vol_.LoadPath();
    }
};

}; // namespace YAML
