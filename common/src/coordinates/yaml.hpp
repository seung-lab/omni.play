#pragma once

#include "yaml-cpp/yaml.h"
#include "utility/yaml/baseTypes.hpp"
#include "coordinates/coordinates.h"

namespace YAML {

template <>
struct convert<om::coords::Global> {
  static Node encode(const om::coords::Global& p) {
    return convert<Vector3f>::encode(p);
  }

  static bool decode(const Node& node, om::coords::Global& p) {
    return convert<Vector3f>::decode(node, p);
  }
};

template <>
struct convert<om::coords::GlobalBbox> {
  static Node encode(const om::coords::GlobalBbox& p) {
    return convert<AxisAlignedBoundingBox<float>>::encode(p);
  }

  static bool decode(const Node& node, om::coords::GlobalBbox& p) {
    return convert<AxisAlignedBoundingBox<float>>::decode(node, p);
  }
};

template <>
struct convert<om::coords::VolumeSystem> {
  static Node encode(const om::coords::VolumeSystem& c) {
    Node node;
    node["dataDimensions"] = c.DataDimensions();
    node["dataResolution"] = c.Resolution();
    node["chunkDim"] = c.ChunkDimensions().x;
    // node["mMipLeafDim"] = c.mMipLeafDim;
    // node["mMipRootLevel"] = c.mMipRootLevel;
    node["absOffset"] = c.AbsOffset();
    return node;
  }

  static bool decode(const Node& node, om::coords::VolumeSystem& c) {
    if (!node.IsMap()) {
      return false;
    }

    auto extent = node["dataExtent"].as<om::coords::GlobalBbox>();
    if (!extent.isEmpty()) {
      c.SetDataDimensions(extent.getDimensions());
    } else {
      c.SetDataDimensions(node["dataDimensions"].as<Vector3i>());
    }

    c.SetResolution(node["dataResolution"].as<Vector3i>(Vector3i::ONE));
    c.SetChunkDimensions(Vector3i(node["chunkDim"].as<int>(128)));
    // in["mMipLeafDim"] >> c.mMipLeafDim;
    // in["mMipRootLevel"] >> c.mMipRootLevel;
    c.SetAbsOffset(node["absOffset"].as<Vector3i>(Vector3i::ZERO));
    c.UpdateRootLevel();
  }
};

}  // namespace YAML
