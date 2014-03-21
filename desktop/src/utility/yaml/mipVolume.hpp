#pragma once
#include "precomp.h"

#include "volume/omChannelFolder.h"
#include "volume/omSegmentationFolder.h"
#include "volume/omVolumeTypes.hpp"

#include "utility/yaml/coords.h"

namespace YAMLold {

inline Emitter& operator<<(Emitter& out, const om::coords::VolumeSystem& c) {
  out << BeginMap;
  out << Key << "dataDimensions" << Value << c.DataDimensions();
  out << Key << "dataResolution" << Value << c.Resolution();
  out << Key << "chunkDim" << Value << c.ChunkDimensions().x;
  // out << Key << "mMipLeafDim" << Value << c.mMipLeafDim;
  out << Key << "mMipRootLevel" << Value << c.RootMipLevel();
  out << Key << "absOffset" << Value << c.AbsOffset();
  out << EndMap;
  return out;
}

inline void operator>>(const Node& in, om::coords::VolumeSystem& c) {
  boost::optional<om::coords::GlobalBbox> extent;
  om::yaml::util::OptionalRead(in, "dataExtent",
                               extent);  // backwards compatibility
  if (extent) {
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
  in["chunkDim"] >> chunkDim;
  c.SetChunkDimensions(Vector3i(chunkDim));
  // in["mMipLeafDim"] >> c.mMipLeafDim;
  Vector3i offset;
  om::yaml::util::OptionalRead(in, "absOffset", offset, Vector3i::ZERO);
  c.SetAbsOffset(offset);
  c.UpdateRootLevel();
}

template <typename VOL>
class mipVolume {
 private:
  VOL& vol_;

 public:
  mipVolume(VOL& vol) : vol_(vol) {}

  void Store(Emitter& out) const {
    out << Key << "id" << Value << vol_.id_;
    out << Key << "custom name" << Value << vol_.customName_;
    out << Key << "note" << Value << vol_.note_;

    out << Key << "coords" << Value << vol_.Coords();

    out << Key << "build state" << Value << vol_.mBuildState;

    const std::string type =
        OmVolumeTypeHelpers::GetTypeAsString(vol_.mVolDataType);
    out << Key << "type" << Value << type;
  }

  void Load(const Node& in) {
    in["id"] >> vol_.id_;
    in["custom name"] >> vol_.customName_;
    in["note"] >> vol_.note_;

    in["coords"] >> vol_.Coords();

    in["build state"] >> vol_.mBuildState;

    std::string volDataType;
    in["type"] >> volDataType;
    vol_.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(
        QString::fromStdString(volDataType));

    vol_.LoadPath();
  }
};

}  // namespace YAMLold
