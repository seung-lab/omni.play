#pragma once
#include "precomp.h"

#include "volume/omVolumeTypes.hpp"
#include "coordinates/volumeSystem.h"
#include "volume/omMipVolume.h"

namespace om {
namespace data {
namespace archive {

template <typename VOL>
class mipVolume {
 private:
  VOL& vol_;

 public:
  mipVolume(VOL& vol) : vol_(vol) {}

  void Store(YAML::Node& node) const {
    node["id"] = vol_.id_;
    node["custom name"] = vol_.customName_;
    node["note"] = vol_.note_;

    node["coords"] = vol_.Coords();

    node["build state"] = vol_.mBuildState;

    const std::string type =
        OmVolumeTypeHelpers::GetTypeAsString(vol_.mVolDataType);
    node["type"] = type;
  }

  void Load(const YAML::Node& in) {
    try {
      vol_.id_ = in["id"].as<om::common::ID>();
      vol_.customName_ = in["custom name"].as<QString>("");
      vol_.note_ = in["note"].as<QString>("");
      vol_.coords_ = in["coords"].as<om::coords::VolumeSystem>();
      vol_.mBuildState = (MipVolumeBuildState)in["build state"].as<int>();

      auto volDataType = in["type"].as<std::string>();
      vol_.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(volDataType);

      vol_.LoadPath();
    }
    catch (YAML::Exception e) {
      log_errors << "Error loading mip Volume: " << e.what();
    }
  }
};

};  // namespace archive
};  // namespace data
};  // namespace om