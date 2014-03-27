#pragma once
#include "precomp.h"

#include "volume/omVolumeTypes.hpp"

namespace om {
namespace data {
namespace archive {

template <typename VOL>
class mipVolume {
 private:
  VOL& vol_;

 public:
  mipVolume(VOL& vol) : vol_(vol) {}

  void Store(YAMLold::Emitter& out) const {
    out << YAMLold::Key << "id" << YAMLold::Value << vol_.id_;
    out << YAMLold::Key << "custom name" << YAMLold::Value << vol_.customName_;
    out << YAMLold::Key << "note" << YAMLold::Value << vol_.note_;

    out << YAMLold::Key << "coords" << YAMLold::Value << vol_.Coords();

    out << YAMLold::Key << "build state" << YAMLold::Value << vol_.mBuildState;

    const std::string type =
        OmVolumeTypeHelpers::GetTypeAsString(vol_.mVolDataType);
    out << YAMLold::Key << "type" << YAMLold::Value << type;
  }

  void Load(const YAMLold::Node& in) {
    in["id"] >> vol_.id_;
    in["custom name"] >> vol_.customName_;
    in["note"] >> vol_.note_;

    in["coords"] >> vol_.Coords();

    in["build state"] >> vol_.mBuildState;

    QString volDataType;
    in["type"] >> volDataType;
    vol_.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(volDataType);

    vol_.LoadPath();
  }
};

};  // namespace archive
};  // namespace data
};  // namespace om