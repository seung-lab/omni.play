#pragma once

#include "precomp.h"
#include "tile/dataSources.hpp"
#include "chunk/dataSources.hpp"

namespace om {
namespace coords {
class VolumeSystem;
}
namespace volume {
class MetadataManager;

class IVolume {
 public:
  virtual tile::TileDS& TileDS() const = 0;
  virtual chunk::ChunkDS& ChunkDS() const = 0;
  virtual MetadataManager& Metadata() const = 0;
  virtual const coords::VolumeSystem& Coords() const = 0;
  virtual const std::string& GUID() const = 0;
  virtual const std::string& Endpoint() const = 0;
  virtual const std::string& Name() const = 0;
};
}
}  // namespace om::volume::
