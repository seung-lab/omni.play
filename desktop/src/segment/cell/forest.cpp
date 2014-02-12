#include "segment/cell/cells.hpp"
#include "segment/cell/forest.h"
#include "segment/cell/storable.hpp"
#include "segment/cell/vcs.h"

namespace om {
namespace cell {

Forest::Forest(segid numNodes, om::segment::Store& store)
    : size_(numNodes + 1), trees_(size_), cells_(size_, store), vcs_() {}

void Forest::checkID(segid id) {
  // for example, when we have 10 nodes, valid id are [1,10]
  // size = numNodes+1
  bool valid = id > 0 && id < size_;
  if (!valid) {
    throw om::ArgException("invalid id", id);
  }
}

bool Forest::Join(segid parentSegID, segid childSegID, float edgeWeight) {
  // check valid join conditions
  checkID(parentSegID);
  checkID(childSegID);
  segid p = Root(parentSegID);
  segid c = Root(childSegID);
  if (p == c) {
    log_debugs(unknown) << "ERROR: same cell";
    return false;
  }

  if (cells_.Valid(p) || cells_.Valid(c)) {
    log_debugs(unknown) << "ERROR: can't join validated cell";
    return false;
  }

  // do the tree join
  trees_.Join(parentSegID, c, edgeWeight);

  // do the cell join
  auto old = cells_.Join(p, c);

  // version control
  // vcs_->???

  return true;
}

boost::optional<edge> Forest::Split(segid id) {
  // check valid split conditions
  checkID(id);
  segid root = Root(id);
  if (root == id) {
    log_debugs(unknown) << "ERROR: can't split root segment";
    return boost::optional<edge>();
  }

  if (cells_.Valid(id)) {
    log_debugs(unknown) << "ERROR: can't split validated cell";
    return boost::optional<edge>();
  }

  // do the tree split
  auto edge = trees_.Split(id);

  // do the cell split
  auto old = cells_.Split(trees_, root, id);

  // version control
  // vcs_->???

  return edge;
}

void Forest::Resize(uint32_t numNodes) {
  if (numNodes == size_ - 1) {
    return;
  }
  if (numNodes < size_ - 1) {
    throw om::ArgException("can't shrink");
  }
  size_ = numNodes + 1;
  trees_.Resize(size_);
  cells_.Resize(size_);
}

std::shared_ptr<om::Storable> Forest::Storable() {
  auto ret = std::make_shared<om::Storable>();
  // TODO: do something!
  return ret;
}
}
}  // om::cell::
