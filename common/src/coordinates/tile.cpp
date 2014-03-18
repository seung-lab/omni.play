#include "precomp.h"
#include "coordinates/tile.h"

namespace om {
namespace coords {

Tile::Tile(const Chunk& cc, common::ViewType view, uint8_t depth)
    : chunk_(cc), viewType_(view), depth_(depth) {}

Tile Tile::Downsample() const {
  int newDepth = (depth_ + (twist(chunk_, viewType_).z % 2) * 128) / 2;
  return Tile(chunk_.ParentCoord(), viewType_, newDepth);
}

std::string Tile::keyStr() const {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}

std::ostream& operator<<(std::ostream& out, const Tile& c) {
  out << "[" << c.chunk() << ", " << c.viewType() << ", " << int(c.depth())
      << "]";

  return out;
}
}
}  // namespace om::coords::
