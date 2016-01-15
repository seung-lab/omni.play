#include "segment/cell/forest.h"
#include "segment/lowLevel/graph.h"
#include "utility/omStringHelpers.h"
#include "segment/lowLevel/cacheRootIDs.hpp"
#include "segment/lowLevel/children.hpp"

namespace om {
namespace segment {
Graph::Graph(common::SegID max)
    : forest_(new OmDynamicForestCache(max + 1)),
      cache_(new CacheRootIDs(forest_.get(), max + 1)),
      children_(new Children(max + 1)) {}

Graph::~Graph() {}

void Graph::Cut(const common::SegID segID) { forest_->Cut(segID); }
void Graph::Join(const common::SegID child, const common::SegID parent) {
  forest_->Join(child, parent);
}

common::SegID Graph::Root(const common::SegID segID) {
  if (forest_->Batch()) {
    return forest_->Root(segID);
  } else {
    return cache_->Root(segID, forest_->Freshness());
  }
}

void Graph::Resize(common::SegID newMax) {
  forest_->Resize(newMax + 1);
  // cellForest_->Resize(newMax);
  children_->Resize(newMax + 1);
}

void Graph::SetBatch(bool batchMode) { forest_->SetBatch(batchMode); }

void Graph::ClearCache() { forest_->ClearCache(); }
} //namespace segment
} //namespace om
