#pragma once

#include <vector>
#include "defs.h"

namespace om {
namespace cell {

/*************
 *
 * Cell
 * --is a root segment
 * --almost always has child segments
 *   --may not have children if it was a leaf node that was split off
 * --maintains metadata related to the Cell:
 *   --valid status of Cell (i.e. Cell can't be split, or joined to other Cells)
 *   --total number of voxels in the Cell (root + children)
 *   --a vector of all segids in the Cell (root + children)
 * --is a persistent data structure (immutable after creation)
 ************/
class Cell {
 public:
  segid id;
  bool valid;
  uint64_t totalNumVoxels;
  std::vector<segid> nodes;

  inline void addNode(segid id, uint64_t size) {
    nodes.push_back(id);
    totalNumVoxels += size;
  }

  inline uint32_t NumNodes() const { return nodes.size(); }
};
}
}  // om::cell::
