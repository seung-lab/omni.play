#pragma once

#include "common/common.h"
#include "common/enums.hpp"

#include "coordinates/chunk.h"

namespace om {
namespace coords {

class Tile {
 public:
  Tile(const Chunk &, common::ViewType, uint8_t);

  Tile Downsample() const;

  std::string keyStr() const;

  friend bool operator==(const Tile &t1, const Tile &t2) {
    return t1.chunk_ == t2.chunk_ && t1.viewType_ == t2.viewType_ &&
           t1.depth_ == t2.depth_;
  }

 private:
  friend std::ostream &operator<<(std::ostream &, const Tile &);

  PROP_CONST_REF(Chunk, chunk);
  PROP_CONST_REF(common::ViewType, viewType);
  PROP_CONST_REF(uint8_t, depth);
};
}
}  // namespace om::coords::

namespace std {
template <>
struct hash<om::coords::Tile> {
  size_t operator()(const om::coords::Tile &t) const {
    std::size_t h1 = std::hash<om::coords::Chunk>()(t.chunk());
    std::size_t h2 = std::hash<int>()((int)t.viewType());
    std::size_t h3 = std::hash<uint8_t>()(t.depth());
    return h1 ^ (h2 << 1) ^ (h3 << 1);
  }
};
}