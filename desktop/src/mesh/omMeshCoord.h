#pragma once

/*
 *
 *	Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "coordinates/chunkCoord.h"
#include "common/common.h"

class OmMeshCoord {
 public:
  OmMeshCoord();
  OmMeshCoord(const om::chunkCoord&, om::common::SegID);

  void operator=(const OmMeshCoord& rhs);
  bool operator==(const OmMeshCoord& rhs) const;
  bool operator!=(const OmMeshCoord& rhs) const;
  bool operator<(const OmMeshCoord& rhs) const;

  om::chunkCoord MipChunkCoord;
  om::common::SegID DataValue;

  const om::chunkCoord& Coord() const { return MipChunkCoord; }

  om::common::SegID SegID() const { return DataValue; }

  friend std::ostream& operator<<(std::ostream& out, const OmMeshCoord& in);
};
