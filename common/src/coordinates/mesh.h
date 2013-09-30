#pragma once

#include "common/core.hpp"
#include "coordinates/chunk.h"

namespace om {
namespace coords {

class Mesh {
 public:
  Mesh(const Chunk&, common::SegID);
  Mesh(const Mesh& rhs);

  bool operator==(const Mesh& rhs) const;
  bool operator!=(const Mesh& rhs) const;
  bool operator<(const Mesh& rhs) const;

  std::string keyStr() const;

 private:
  friend std::ostream& operator<<(std::ostream& out, const Mesh& in);

  PROP_CONST_REF(Chunk, mipChunkCoord);
  PROP_CONST_REF(common::SegID, segID);
};
}
}  // namespace om::coords::

namespace std {
template <> struct hash<om::coords::Mesh> {
  size_t operator()(const om::coords::Mesh& m) const {
    std::size_t h1 = std::hash<om::coords::Chunk>()(m.mipChunkCoord());
    std::size_t h2 = std::hash<om::common::SegID>()(m.segID());
    return h1 ^ (h2 << 1);
  }
};
}