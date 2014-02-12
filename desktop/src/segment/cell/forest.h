#pragma once

#include <boost/optional.hpp>

#include "defs.h"
#include "segment/cell/cells.hpp"
#include "segment/cell/trees.hpp"
#include "segment/cell/vcs.h"

namespace om {

namespace segment {
class Store;
}

class Storable;

namespace cell {

class Cells;
class Trees;
class VCS;

/***********
 * Forest:
 * --is a collection of Cells
 * --maintains the trees that maps segid --> Cell
 * --knows how to join and split Cells
 * --knows how to mark Cells as Valid (i.e. non-Joinable, non-Splittable)
 *
 * TODO:
 * --setup support for marking cells as valid (prevent join/split, etc.)
 * --use C++11 move semantics and refs instead of std::shared_ptr, thus
 *   eliminating cell_t
 * --when splitting, figure out a better way of finding the children that
 *   belong to the two new cells (keeping lists of children and total voxel
 *   volume here eliminates a lot of code higher up; doing it here as we
 *   split/join hopefully amoritizes the cost....)
 * --testing:
 *   --gmock
 *   --check boundaries segID boundaries (segid == 0, segid == numNodes)
 *   --check valid segments
 **********/

class Forest {
 private:
  uint32_t size_;
  Trees trees_;
  Cells cells_;
  VCS vcs_;

  void checkID(segid id);

 public:
  Forest(segid numNodes, om::segment::Store& store);

  // cell operations
  inline uint64_t Size(segid id) { return cells_.Size(id); }
  inline segid Root(segid id) { return trees_.Root(id); }

  // tree operations
  bool Join(segid parentSegID, segid childSegID, float edgeWeight);
  boost::optional<edge> Split(segid splitSegID);  // return edge split
  void Resize(uint32_t size);

  std::shared_ptr<om::Storable> Storable();
};
}
}  // om::cell::
