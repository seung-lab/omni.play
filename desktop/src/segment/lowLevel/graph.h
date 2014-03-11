#pragma once

#include "common/common.h"
#include "segment/lowLevel/omSegmentLowLevelTypes.h"
#include "threads/taskManagerTypes.h"

class OmSegment;
class OmValidGroupNum;
class OmDynamicForestCache;

namespace om {
namespace cell {
class Forest;
}
namespace segment {
class Store;
class CacheRootIDs;
class Children;

class Graph {
 public:
  Graph(common::SegID max);
  ~Graph();

  void Resize(common::SegID newMax);
  common::SegID Root(const common::SegID segID);
  void Cut(const common::SegID segID);
  void Join(const common::SegID child, const common::SegID parent);
  void SetBatch(bool batchMode);
  void ClearCache();

  Children& SegChildren() const { return *children_; }

 private:
  std::unique_ptr<OmDynamicForestCache> forest_;
  std::unique_ptr<CacheRootIDs> cache_;
  std::unique_ptr<Children> children_;
};
}
}
