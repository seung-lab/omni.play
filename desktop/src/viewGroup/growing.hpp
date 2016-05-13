#pragma once

#include "precomp.h"
#include "segment/types.hpp"
#include "segment/omSegmentSelector.h"

class Growing {
 public:
  const uint32_t BFS_STEP_LIMIT = 1000;

  bool GrowBreadthFirstSearch(OmSegmentSelector& selector, double threshold,
      const std::unordered_map<om::common::SegID,
      std::vector <om::segment::Edge*>>& adjacencyMap)  {
    om::common::SegID startID = selector.GetFocusSegment();
    if (!startID) {
      return false;
    }

    om::common::SegIDList vecToAdd, vecToRemove;
    std::tie(vecToAdd, vecToRemove) =
      thresholdBreadthFirstSearch(startID, threshold, adjacencyMap, selector);
    vecToRemove = removeBreadthFirstSearch(vecToRemove, adjacencyMap, selector);

    selector.InsertSegments(vecToAdd);
    selector.RemoveSegments(vecToRemove);
    return true;
  }

 private:
  om::common::SegID getOtherSegID(om::common::SegID segID,
      om::segment::Edge* edge) {
    om::common::SegID otherSegID;
    if (segID == edge->node2ID) {
      otherSegID = edge->node1ID;
    } else {
      otherSegID = edge->node2ID;
    }
    return otherSegID;
  }

  std::tuple<om::common::SegIDList, om::common::SegIDList>
    thresholdBreadthFirstSearch(om::common::SegID startID,
      double threshold, const std::unordered_map<om::common::SegID,
      std::vector <om::segment::Edge*>>& adjacencyMap,
      OmSegmentSelector& selector) {
    std::queue <om::common::SegID> q;
    om::segment::Edge *currEdge;
    om::common::SegID currSegment, nextSegment;

    log_infos << "Grow BFS : " << startID << " with threshold " << threshold;

    q.push(startID);

    om::common::SegIDList vecToAdd;
    om::common::SegIDList vecToRemove;
    om::common::SegIDSet setToAdd;
    setToAdd.insert(startID);
    vecToAdd.push_back(startID);

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
        nextSegment = getOtherSegID(currSegment, currEdge);

        log_debugs << "BFS for (" << currSegment << ") looking at: " <<
          nextSegment << " (" << currEdge->threshold << ") <? (" <<
          threshold << ")";

        if (setToAdd.find(nextSegment) != setToAdd.end()) {
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
            vecToRemove.push_back(nextSegment);
          }
          continue;
        }

        q.push(nextSegment);
        setToAdd.insert(nextSegment);
        vecToAdd.push_back(nextSegment);
      }
    }
    return std::make_tuple(vecToAdd, vecToRemove);
  }

  om::common::SegIDList removeBreadthFirstSearch(om::common::SegIDList vecToRemove,
      const std::unordered_map<om::common::SegID,
      std::vector <om::segment::Edge*>>& adjacencyMap,
      OmSegmentSelector& selector) {
    std::queue <om::common::SegID> q;
    om::segment::Edge *currEdge;
    om::common::SegID currSegment, nextSegment;

    log_infos << "Remove BFS ";

    om::common::SegIDSet setToRemove;
    for (auto removeID : vecToRemove) {
      q.push(removeID);
    }
    setToRemove.insert(vecToRemove.begin(), vecToRemove.end());

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
        nextSegment = getOtherSegID(currSegment, currEdge);
        uint32_t nextOrderOfAdding = selector.GetOrderOfAdding(nextSegment);

        log_debugs << "BFS Trim (" << currSegment << ") looking at: " <<
          nextSegment << " CurrOrder (" << currOrderOfAdding<< ") <? (" <<
          nextOrderOfAdding << ")";

        if (!selector.IsSegmentSelected(nextSegment)) {
          log_debugs << "segment not selected";
          continue;
        }

        if (setToRemove.find(nextSegment) != setToRemove.end()) {
          log_debugs << "segment already being removed";
          continue;
        }

        if (nextOrderOfAdding && currOrderOfAdding > nextOrderOfAdding) {
          log_debugs << "segment order is before current Segment";
          continue;
        }

        q.push(nextSegment);
        setToRemove.insert(nextSegment);
        vecToRemove.push_back(nextSegment);
      }
    }

    return vecToRemove;
  }
};
