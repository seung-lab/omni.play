#pragma once

#include "precomp.h"
#include "segment/types.hpp"
#include "segment/omSegmentSelector.h"
#include "users/omUsers.h"

class Growing {
 public:
  void GrowBreadthFirstSearch(OmSegmentSelector& selector, double threshold,
      const std::unordered_map<om::common::SegID,
      std::vector <om::segment::Edge*>>& adjacencyMap) {
    std::queue <om::common::SegID> q;
    om::segment::Edge *currEdge;
    om::common::SegID currSegment, nextSegment;

    // set the current selection so we can increment/decrement
    currentThreshold = threshold;
    om::common::SegID selectedSegmentID = selector.GetFocusSegment();
    log_infos << "Grow BFS : " << selectedSegmentID << " with threshold " <<
      currentThreshold;

    q.push(selectedSegmentID);

    om::common::SegIDList vecToAdd;
    om::common::SegIDSet setToAdd;
    setToAdd.insert(selectedSegmentID);
    vecToAdd.push_back(selectedSegmentID);

    int br=0;
    while (!q.empty()) {
      br++;
      currSegment = q.front();

      if (br == 1000) {
          break;
      }

      q.pop();

      auto mapIter = adjacencyMap.find(currSegment);
      if (mapIter == adjacencyMap.end()) {
        continue;
      }

      uint32_t currOrderOfAdding = selector.GetOrderOfAdding(currSegment);

      for ( int i = 0; i < mapIter->second.size(); i++ ) {
        currEdge = mapIter->second[i];

        if (currSegment == currEdge->node2ID) {
          nextSegment = currEdge->node1ID;
        } else {
          nextSegment = currEdge->node2ID;
        }

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
    selector.InsertSegments(vecToAdd);
  }

  void Trim(OmSegmentSelector& selector, om::common::SegID segmentID,
      const std::unordered_map<om::common::SegID,
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

        if (currSegment == currEdge->node2ID) {
          nextSegment = currEdge->node1ID;
        } else {
          nextSegment = currEdge->node2ID;
        }

        uint32_t nextOrderOfAdding = selector.GetOrderOfAdding(nextSegment);

        log_debugs << "TRIM for (" << nextSegment << ") looking at: " <<
                    currSegment << " - " << nextSegment <<
                    " (" << nextOrderOfAdding <<
                    ") vs minOrderOfAdding (" << minOrderOfAdding << ")";

        if (nextOrderOfAdding <= minOrderOfAdding) {
          log_debugs << "order of segment is before selected segment";
          continue;
        }

        if (!nextOrderOfAdding) {
          log_debugs << "Segment is not selected";
          continue;
        }

        if (setToRemove.find(nextSegment) != setToRemove.end() ) {
          log_debugs << "Segment already included for removal";
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
      bool isGrowing, const std::unordered_map<om::common::SegID,
      std::vector <om::segment::Edge*>>& adjacencyMap) {
    if (isGrowing) {
      currentThreshold += .001;
    } else {
      currentThreshold -= .001;
    }

    // enforce limits to the threshold
    if (currentThreshold > 1) {
      currentThreshold = 1;
    }

    if (currentThreshold < 0) {
      currentThreshold = 0;
    }

    om::common::SegID selectedSegmentID = selector.GetFocusSegment();
    if (selectedSegmentID) {
      selector.selectJustThisSegment(selectedSegmentID, true);
      GrowBreadthFirstSearch(selector, currentThreshold, adjacencyMap);
      selector.SetFocusSegment(selectedSegmentID);
    }
  }

 private:

  double currentThreshold;
};
