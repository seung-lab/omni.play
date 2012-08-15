#pragma once

#include "yaml-cpp/yaml.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/fs/omFileNames.hpp"
#include "volume/omChannelFolder.h"
#include "volume/omSegmentationFolder.h"

#include <QString>
#include <QFile>

namespace YAML {

template <typename VOL>
class mipVolume {
private:
    VOL& vol_;

public:
    mipVolume(VOL& vol)
    : vol_(vol)
    {}

    void Store(Emitter& out) const
    {
        out << Key << "id" << Value << vol_.id_;
        out << Key << "custom name" << Value << vol_.customName_;
        out << Key << "note" << Value << vol_.note_;

        out << Key << "coords" << Value << vol_.Coords();

        out << Key << "build state" << Value << vol_.mBuildState;

        const std::string type =
        OmVolumeTypeHelpers::GetTypeAsString(vol_.mVolDataType);
        out << Key << "type" << Value << type;
    }

    void Load(const Node& in)
    {
        in["id"] >> vol_.id_;
        in["custom name"] >> vol_.customName_;
        in["note"] >> vol_.note_;

        in["coords"] >> vol_.Coords();

        in["build state"] >> vol_.mBuildState;

        std::string volDataType;
        in["type"] >> volDataType;
        vol_.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(QString::fromStdString(volDataType));

        vol_.LoadPath();
    }
};

} // namespace YAML