#pragma once

#include "precomp.h"
#include "segment/types.hpp"
#include "segment/omSegmentSelector.h"

class Growing {
 public:
  const uint32_t BFS_STEP_LIMIT = 1000;

  /*
   * BFS grow and find all segments not yet selected that are above threshold
   * returns: tuple of newly discovered segments (seed exclusive) and
   *  seed segments of connected segments that are below the threshold
   */
  std::tuple<om::common::SegIDList, om::common::SegIDList>
    FindNotSelected(om::common::SegID seedID, double threshold,
        const std::unordered_map<om::common::SegID,
          std::vector <om::segment::Edge*>>& adjacencyMap,
          OmSegmentSelector& selector) {
    return findNotSelected(seedID, threshold, adjacencyMap, selector);
  }

  /*
   * BFS grow and find all segments that are currently selected (seed exclusive)
   * returns: list of all connected segments
   */
  om::common::SegIDList FindSelected(om::common::SegIDList seedIDs,
        OmSegmentSelector& selector, const std::unordered_map<om::common::SegID,
          std::vector <om::segment::Edge*>>& adjacencyMap) {
    return findSelected(seedIDs, adjacencyMap, selector);
  }

 private:
  std::tuple<om::common::SegIDList, om::common::SegIDList>
    findNotSelected(om::common::SegID seedID,
      double threshold, const std::unordered_map<om::common::SegID,
      std::vector <om::segment::Edge*>>& adjacencyMap,
      OmSegmentSelector& selector) {
    std::queue <om::common::SegID> q;
    om::segment::Edge *currEdge;
    om::common::SegID currSegment, nextSegment;

    log_infos << "Grow BFS : " << seedID << " with threshold " << threshold;

    q.push(seedID);

    om::common::SegIDList newIDs, pruneSeedIDs;
    om::common::SegIDSet visited;
    visited.insert(seedID);

    int br=0;
    while (!q.empty()) {
      br++;
      if (br >= BFS_STEP_LIMIT) {
          break;
      }

      currSegment = q.front();
      q.pop();

      uint32_t currOrderOfAdding = selector.GetOrderOfAdding(currSegment);

      auto mapIter = adjacencyMap.find(currSegment);
      if (mapIter == adjacencyMap.end()) {
        continue;
      }

      for ( int i = 0; i < mapIter->second.size(); i++ ) {
        currEdge = mapIter->second[i];
        nextSegment = currEdge->otherNodeID(currSegment);

        log_debugs << "BFS FindNotSel (" << currSegment << ") looking at: " <<
          nextSegment << " (" << currEdge->threshold << ") <? (" <<
          threshold << ")";

        if (selector.IsBlacklistSegment(nextSegment)) {
          log_debugs << "segment is blacklisted";
          continue;
        }

        if (visited.find(nextSegment) != visited.end()) {
          log_debugs << "segment already being added";
          continue;
        }

        uint32_t nextOrderOfAdding = selector.GetOrderOfAdding(nextSegment);
        if (nextOrderOfAdding && currOrderOfAdding > nextOrderOfAdding) {
          log_debugs << "segment already added before grow";
          continue;
        }

        if (currEdge->threshold < threshold) {
          log_debugs << "threshold is too small";
          // this asssumes that we already checked that it is valid ordering
          if (selector.IsSegmentSelected(nextSegment)) {
            pruneSeedIDs.push_back(nextSegment);
          }
          continue;
        }

        q.push(nextSegment);
        visited.insert(nextSegment);
        newIDs.push_back(nextSegment);
      }
    }
    return std::make_tuple(newIDs, pruneSeedIDs);
  }

  om::common::SegIDList findSelected(om::common::SegIDList seedIDs,
      const std::unordered_map<om::common::SegID,
        std::vector <om::segment::Edge*>>& adjacencyMap,
      OmSegmentSelector& selector) {
    std::queue <om::common::SegID> q;
    om::segment::Edge *currEdge;
    om::common::SegID currSegment, nextSegment;

    log_infos << "Remove BFS ";

    for (auto removeID : seedIDs) {
      q.push(removeID);
    }

    om::common::SegIDList reachedSelectedIDs;
    om::common::SegIDSet visited;

    visited.insert(seedIDs.begin(), seedIDs.end());

    int br=0;
    while (!q.empty()) {
      br++;
      if (br >= BFS_STEP_LIMIT) {
          break;
      }

      currSegment = q.front();
      q.pop();

      uint32_t currOrderOfAdding = selector.GetOrderOfAdding(currSegment);

      auto mapIter = adjacencyMap.find(currSegment);
      if (mapIter == adjacencyMap.end()) {
        continue;
      }

      for ( int i = 0; i < mapIter->second.size(); i++ ) {
        currEdge = mapIter->second[i];
        nextSegment = currEdge->otherNodeID(currSegment);
        uint32_t nextOrderOfAdding = selector.GetOrderOfAdding(nextSegment);

        log_debugs << "BFS FindSelected (" << currSegment << ") looking at: " <<
          nextSegment << " CurrOrder (" << currOrderOfAdding<< ") <? (" <<
          nextOrderOfAdding << ")";

        if (!selector.IsSegmentSelected(nextSegment)) {
          log_debugs << "segment not selected";
          continue;
        }

        if (visited.find(nextSegment) != visited.end()) {
          log_debugs << "segment already being removed";
          continue;
        }

        if (nextOrderOfAdding && currOrderOfAdding > nextOrderOfAdding) {
          log_debugs << "segment order is before current Segment";
          continue;
        }

        q.push(nextSegment);
        visited.insert(nextSegment);
        reachedSelectedIDs.push_back(nextSegment);
      }
    }

    return reachedSelectedIDs;
  }
};
