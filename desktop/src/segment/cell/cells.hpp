#pragma once

#include <unordered_map>
#include "segment/cell/cell.hpp"
#include "segment/cell/trees.hpp"
#include "segment/omSegment.h"
#include "segment/lowLevel/store.h"

extern template class std::vector<bool>;

namespace om {
namespace cell {

class SegmentCache {
  om::segment::Store& store_;
  std::unordered_map<segid, OmSegment*> cache_;

 public:
  SegmentCache(om::segment::Store& store) : store_(store) {}

  inline OmSegment* GetSegment(segid id) {
    auto it = cache_.find(id);
    if (cache_.end() == it) {
      return cache_[id] = store_.GetSegment(id);
    }
    return it->second;
  }
};

struct CellsJoined {
  Cell parent;
  Cell child;
};

class Cells {
  std::vector<Cell> cells_;
  std::vector<bool> allocated_;

  SegmentCache segmentCache_;

  inline uint64_t getSegmentSize(segid id) {
    return segmentCache_.GetSegment(id)->size();
  }

  inline Cell& setupSingleCell(segid id) {
    auto& c = cells_[id];
    c.id = id;
    c.valid = false;
    c.totalNumVoxels = getSegmentSize(id);
    c.nodes = { id };
    allocated_[id] = true;
    return c;
  }

  inline Cell& setupRootCell(segid id, uint32_t reserve) {
    auto& c = cells_[id];
    c.id = id;
    c.valid = false;
    c.totalNumVoxels = 0;
    c.nodes.clear();
    c.nodes.reserve(reserve);
    allocated_[id] = true;
    return c;
  }

  inline Cell& at(segid id) {
    if (!allocated_[id]) {
      throw om::ArgException("Cell was not yet initialized", id);
    }
    return cells_[id];
  }

  inline Cell& get(segid id) {
    if (!allocated_[id]) {
      return setupSingleCell(id);
    }
    return cells_[id];
  }

 public:
  Cells(uint32_t size, om::segment::Store& store)
      : cells_(size), allocated_(size, false), segmentCache_(store) {}

  void Resize(uint32_t size) {
    cells_.resize(size);
    allocated_.resize(size, false);
  }

  inline uint64_t Size(segid id) {
    if (!allocated_[id]) {
      return 0;
    }
    return cells_[id].totalNumVoxels;
  }

  inline bool Valid(segid id) const {
    if (!allocated_[id]) {
      return false;
    }
    return cells_[id].valid;
  }

  std::shared_ptr<CellsJoined> Join(segid pid, segid cid) {
    auto& p = get(pid);
    auto& c = get(cid);
    auto old = std::make_shared<CellsJoined>(CellsJoined {
      p, c
    });

    p.totalNumVoxels += c.totalNumVoxels;
    p.nodes.reserve(p.nodes.size() + c.nodes.size());
    p.nodes.insert(p.nodes.end(), c.nodes.begin(), c.nodes.end());

    allocated_[cid] = false;  // child cell no longer exists on its own

    return old;
  }

  std::shared_ptr<Cell> Split(Trees& trees, segid root, segid id) {
    auto& p = at(root);
    const auto old = std::make_shared<Cell>(p);
    const auto& nodes = old->nodes;

    setupRootCell(root, nodes.size());
    auto& c = setupRootCell(id, nodes.size());

    // nodes don't (easily) know who their children are :(
    // TODO: make better
    for (auto& n : nodes) {
      auto size = getSegmentSize(n);

      if (p.id == trees.DFRoot(n)) {
        p.addNode(n, size);
      } else {
        c.addNode(n, size);
      }
    }

    return old;
  }
};
}
}  // om::cell::
