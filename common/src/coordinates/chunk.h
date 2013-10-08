#pragma once

/*
 * chunk represents a location in Mip Space given by a level (or mip
 * resolution),
 *  and an x,y,z coordinate.  This is stored in a four element tuple as [level,
 * (x,y,z)].
 *
 * Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "vmmlib/vmmlib.h"
using namespace vmml;

#include <iostream>

namespace om {
namespace coords {

class dataBbox;
class data;
class volumeSystem;

class chunk {

 public:
  chunk();
  chunk(int, const Vector3i&);
  chunk(int level, int, int, int);

  //static const chunk NULL_COORD;

  std::string getCoordsAsString() const;

  //property
  inline bool IsLeaf() const { return 0 == Level; }

  //family coordinate methods
  chunk ParentCoord() const;
  chunk PrimarySiblingCoord() const;
  void SiblingCoords(chunk* pSiblings) const;
  chunk PrimaryChildCoord() const;
  void ChildrenCoords(chunk* pChildren) const;

  //access
  inline int getLevel() const { return Level; }
  inline int X() const { return Coordinate.x; }
  inline int Y() const { return Coordinate.y; }
  inline int Z() const { return Coordinate.z; }

  data toData(const volumeSystem*) const;
  dataBbox chunkBoundingBox(const volumeSystem*) const;
  uint64_t chunkPtrOffset(const volumeSystem*, int64_t) const;
  int sliceDepth(const volumeSystem*, global, common::viewType) const;

  //operators
  void operator=(const chunk& rhs);
  bool operator==(const chunk& rhs) const;
  bool operator!=(const chunk& rhs) const;
  bool operator<(const chunk& rhs) const;

  int Level;
  Vector3i Coordinate;

  friend std::ostream& operator<<(std::ostream& out, const chunk& in);
};

}  // namespace coords
}  // namespace om
