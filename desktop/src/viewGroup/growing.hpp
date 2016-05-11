#pragma once

#include "precomp.h"
#include "segment/types.hpp"
#include "segment/omSegmentSelector.h"

class Growing {
 public:

  void GrowBreadthFirstSearch(OmSegmentSelector& selector, double threshold,
      const std::unordered_map<om::common::SegID,
      std::vector <om::segment::Edge*>>& adjacencyMap) {
    std::queue <om::common::SegID> q;
    om::segment::Edge *currEdge;
    om::common::SegID currSegment, nextSegment;

    // set the current selection so we can increment/decrement
    om::common::SegID selectedSegmentID = selector.GetFocusSegment();
    log_infos << "Grow BFS : " << selectedSegmentID << " with threshold " <<
      threshold;

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

        nextSegment = getOtherSegID(currSegment, currEdge);

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
};
