#pragma once

#include "precomp.h"
#include "segment/types.hpp"
#include "segment/omSegmentSelector.h"

class Growing {
 public:
  void GrowBreadthFirstSearch(OmSegmentSelector& selector, double threshold,
      const std::unordered_map<om::common::SegID,
      std::vector <om::segment::Edge*>>& adjacencyMap)  {
    om::common::SegID startID = selector.GetFocusSegment();
    if (!startID) {
      return;
    }

    om::common::SegIDList vecToAdd =
      thresholdBreadthFirstSearch(startID, threshold, adjacencyMap, selector);
    /*
     *std::cout << "Trying to add: ";
     *for (auto a : vecToAdd) {
     *  std::cout<< a << ", ";
     *}
     *std::cout << std::endl;
     */

    selector.InsertSegments(vecToAdd);
  }

  void Trim(OmSegmentSelector& selector, om::common::SegID segmentID,
      double threshold, const std::unordered_map<om::common::SegID,
        std::vector <om::segment::Edge*>>& adjacencyMap) {
    om::segment::Edge *currEdge;
    om::common::SegID currSegment, nextSegment;

    uint32_t minOrderOfAdding = -1;

    if (selector.GetOrderOfAdding(segmentID)) {
      minOrderOfAdding = selector.GetOrderOfAdding(segmentID);
    }

    log_infos << "Trying to trim " << segmentID << " minOrderOfAdding is " <<
      minOrderOfAdding;

    std::queue <om::common::SegID> q;
    q.push(segmentID);

    om::common::SegIDSet setToRemove;

    while (!q.empty()) {
      currSegment = q.front();

      q.pop();

      auto mapIter = adjacencyMap.find(currSegment);
      if (mapIter == adjacencyMap.end()) {
        continue;
      }

      for (int i = 0; i < mapIter->second.size(); i++) {
        currEdge = mapIter->second[i];

        nextSegment = getOtherSegID(currSegment, currEdge);

        uint32_t nextOrderOfAdding = selector.GetOrderOfAdding(nextSegment);

        log_debugs << "TRIM for (" << nextSegment << ") looking at: " <<
                    currSegment << " - " << nextSegment <<
                    " (" << nextOrderOfAdding <<
                    ") vs minOrderOfAdding (" << minOrderOfAdding << ")";

        if (!selector.IsSegmentSelected(currSegment)) {
          log_debugs << "order of segment is before selected segment";
          continue;
        }

        if (!nextOrderOfAdding) {
          log_debugs << "Segment is not selected";
          continue;
        }

        if (setToRemove.find(nextSegment) != setToRemove.end()) {
          log_debugs << "Segment already included for removal";
          continue;
        }

        if (setToRemove.find(nextSegment) != setToRemove.end()) {
          log_debugs << "Already removed";
          continue;
        }

        q.push(nextSegment);
        log_debugs << "Removing segment" << nextSegment;

        setToRemove.insert(nextSegment);
      }
    }

    selector.RemoveTheseSegments(setToRemove);
  }

  void GrowIncremental(OmSegmentSelector& selector,
      bool isGrowing, double threshold, const std::unordered_map<om::common::SegID,
      std::vector <om::segment::Edge*>>& adjacencyMap) {
    om::common::SegID selectedSegmentID = selector.GetFocusSegment();
    if (selectedSegmentID) {
      selector.selectJustThisSegment(selectedSegmentID, true);
      GrowBreadthFirstSearch(selector, threshold, adjacencyMap);
      selector.SetFocusSegment(selectedSegmentID);
    }
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

  om::common::SegIDList thresholdBreadthFirstSearch(om::common::SegID startID,
      double threshold, const std::unordered_map<om::common::SegID,
      std::vector <om::segment::Edge*>>& adjacencyMap,
      OmSegmentSelector& selector) {
    std::queue <om::common::SegID> q;
    om::segment::Edge *currEdge;
    om::common::SegID currSegment, nextSegment;

    log_infos << "Grow BFS : " << startID << " with threshold " << threshold;

    q.push(startID);

    om::common::SegIDList vecToAdd;
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

        if (currEdge->threshold < threshold) {
          log_debugs << "threshold is too small";
          continue;
        }

        if (setToAdd.find(nextSegment) != setToAdd.end()) {
          log_debugs << "segment already added";
          continue;
        }

        // don't traverse previously selected edges
        uint32_t nextOrderOfAdding = selector.GetOrderOfAdding(nextSegment);
        if (nextOrderOfAdding && currOrderOfAdding > nextOrderOfAdding) {
          log_debugs << "segment previously added already";
          continue;
        }

        q.push(nextSegment);
        setToAdd.insert(nextSegment);
        vecToAdd.push_back(nextSegment);
      }
    }
    return vecToAdd;
  }

  const uint32_t BFS_STEP_LIMIT = 1000;
};
