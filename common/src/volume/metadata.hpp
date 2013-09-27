#pragma once

#include "common/common.h"
#include "coordinates/coordinates.h"
#include "utility/UUID.hpp"

namespace om {
namespace volume {

struct Metadata {
  std::string Name;
  utility::UUID UUID;
  coords::GlobalBbox Bounds;
  Vector3i Resolution;
  int ChunkDim;
  int RootMipLevel;
  common::DataType DataType;
  common::ObjectType VolumeType;
  uint32_t NumSegments;
  uint32_t MaxSegments;
  uint32_t NumEdges;

  Metadata()
      : Name(""),
        UUID(),
        Bounds(coords::GlobalBbox::UNITBOX),
        Resolution(Vector3i::ONE),
        ChunkDim(128),
        RootMipLevel(1),
        DataType(common::DataType::UNKNOWN),
        VolumeType(common::ObjectType::CHANNEL),
        NumSegments(0),
        MaxSegments(0),
        NumEdges(0) {}

  friend bool operator==(const Metadata& a, const Metadata& b) {
    return std::tie(a.Name, a.UUID.Str(), a.Bounds, a.Resolution, a.ChunkDim,
                    a.RootMipLevel, a.DataType, a.VolumeType, a.NumSegments,
                    a.MaxSegments, a.NumEdges) ==
           std::tie(b.Name, b.UUID.Str(), b.Bounds, b.Resolution, b.ChunkDim,
                    b.RootMipLevel, b.DataType, b.VolumeType, b.NumSegments,
                    b.MaxSegments, b.NumEdges);
  }

  friend std::ostream& operator<<(std::ostream& out, const Metadata& m) {
    return out << "{" << std::endl << "Name: " << m.Name << std::endl
               << "UUID: " << m.UUID.Str() << std::endl
               << "Bounds: " << m.Bounds << std::endl
               << "Resolution: " << m.Resolution << std::endl
               << "ChunkDim: " << m.ChunkDim << std::endl
               << "RootMipLevel: " << m.RootMipLevel << std::endl
               << "DataType: " << m.DataType << std::endl
               << "VolumeType: " << m.VolumeType << std::endl
               << "NumSegments: " << m.NumSegments << std::endl
               << "MaxSegments: " << m.MaxSegments << std::endl
               << "NumEdges: " << m.NumEdges << std::endl << "}" << std::endl;
  }
};
}
}  // namespace om::volume::
