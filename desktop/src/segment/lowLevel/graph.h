#pragma once

#include "common/common.h"

#include "segment/lowLevel/omSegmentLowLevelTypes.h"
#include "threads/taskManagerTypes.h"

class OmSegmentSelector;

class OmSegment;

class OmSegmentSelection;
class OmValidGroupNum;
class OmDynamicForestCache;

namespace om {
namespace cell {
  class Forest;
}
  static const int omsegmentgraph_newlevel = -1 ;
  static const int step                    = 100;
  static const int max_nos                 = 10 ;

  typedef std::vector<OmSegment*> some_type[max_nos][max_nos*2+2][max_nos];

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
} //namespace segment
} //namespace om
