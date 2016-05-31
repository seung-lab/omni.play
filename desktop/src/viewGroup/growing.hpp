#pragma once

#include "precomp.h"
#include "segment/types.hpp"
#include "segment/omSegmentSelector.h"

class Growing {
 public:
  static const uint32_t BFS_STEP_LIMIT;

  /*
   * BFS grow and find all segments not yet selected that are above threshold
   * returns: tuple of newly discovered segments (seed exclusive) and
   *  seed segments of connected segments that are below the threshold
   */
  std::tuple<om::common::SegIDList, om::common::SegIDList>
    FindNotSelected(om::common::SegID seedID, double threshold,
        OmSegmentSelector& selector, const std::unordered_map<om::common::SegID,
          std::vector <om::segment::Edge*>>& adjacencyMap,
          const uint32_t maxDepth = BFS_STEP_LIMIT);

  /*
   * BFS grow and find all segments that are currently selected (seed exclusive)
   * returns: list of all connected segments
   */
  om::common::SegIDList FindSelected(om::common::SegIDList seedIDs,
        OmSegmentSelector& selector, const std::unordered_map<om::common::SegID,
          std::vector <om::segment::Edge*>>& adjacencyMap,
          const uint32_t maxDepth = BFS_STEP_LIMIT);
};
