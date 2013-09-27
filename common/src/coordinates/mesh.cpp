#include "common/common.h"
#include "coordinates/coordinates.h"

namespace om {
namespace coords {

Mesh::Mesh(const Chunk& mipChunkCoord, common::SegID segID)
    : mipChunkCoord_(mipChunkCoord), segID_(segID) {}

Mesh::Mesh(const Mesh& m)
    : mipChunkCoord_(m.mipChunkCoord_), segID_(m.segID_) {}

bool Mesh::operator==(const Mesh& rhs) const {
  return mipChunkCoord_ == rhs.mipChunkCoord_ && segID_ == rhs.segID_;
}

bool Mesh::operator!=(const Mesh& rhs) const {
  return mipChunkCoord_ != rhs.mipChunkCoord_ || segID_ != rhs.segID_;
}

/* comparitor for key usage */
bool Mesh::operator<(const Mesh& rhs) const {
  if (mipChunkCoord_ != rhs.mipChunkCoord_) {
    return (mipChunkCoord_ < rhs.mipChunkCoord_);
  }

  return (segID_ < rhs.segID_);
}

std::string Mesh::keyStr() const {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}

std::ostream& operator<<(std::ostream& out, const Mesh& c) {
  out << "(" << c.segID_ << ", " << c.mipChunkCoord_ << ")";
  return out;
}
}
}  // namespace om::coords::
