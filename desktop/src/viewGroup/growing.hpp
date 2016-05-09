#pragma once

#include "precomp.h"
#include "segment/types.hpp"
#include "segment/omSegmentSelector.h"
#include "users/omUsers.h"

class Growing {
 public:
  void GrowBreadthFirstSearch(OmSegmentSelector& selector, om::common::SegID segmentID,
    const std::unordered_map<om::common::SegID,
     std::vector <om::segment::Edge*>>& adjacencyMap) {
    std::queue <om::common::SegID> q;
    om::segment::Edge *currEdge;
    om::common::SegID currSegment, nextSegment;
    double threshold = OmProject::Globals().Users().UserSettings().getASThreshold();

    q.push(segmentID);

    om::common::SegIDList vecToAdd;
    om::common::SegIDSet setToAdd;
    setToAdd.insert(segmentID);
    vecToAdd.push_back(segmentID);

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

      for ( int i = 0; i < mapIter->second.size(); i++ ) {
        currEdge = mapIter->second[i];

        if (currSegment == currEdge->node2ID) {
          nextSegment = currEdge->node1ID;
        } else {
          nextSegment = currEdge->node2ID;
        }

        log_debugs << "BFS for (" << currSegment << ") looking at: " << nextSegment <<
                      " (" << currEdge->threshold << ") <? (" << threshold << ")";

        if (currEdge->threshold < threshold) {
          continue;
        }

        if (selector.IsSegmentSelected(nextSegment)) {
          continue;
        }
        if (setToAdd.find(nextSegment) != setToAdd.end()) {
          continue;
        }

        // don't traverse previously selected edges
        if (selector.GetOrderOfAdding(nextSegment)) {
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

    uint32_t mini = -1;

    if (!selector.GetOrderOfAdding(segmentID)) {
      auto mapIter = adjacencyMap.find(currSegment);
      if (mapIter != adjacencyMap.end()) {
        for (int i = 0; i < mapIter->second.size(); i++) {
          currEdge = mapIter->second[i];

          if (segmentID == currEdge->node2ID) {
            nextSegment = currEdge->node1ID;
          } else {
            nextSegment = currEdge->node2ID;
          }

          if (selector.GetOrderOfAdding(nextSegment) &&
              (mini == -1 || selector.GetOrderOfAdding(nextSegment) < mini)) {
            mini = selector.GetOrderOfAdding(nextSegment);
          }
        }
      }
    } else {
      mini = selector.GetOrderOfAdding(segmentID);
    }

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

        log_debugs << "TRIM for (" << nextSegment << ") looking at: " <<
                    currEdge->node1ID << " - " << currEdge->node2ID <<
                    " (" << selector.GetOrderOfAdding(nextSegment) << ") <=? mini (" <<
                      mini << ")";

        if (selector.GetOrderOfAdding(nextSegment) <= mini) {
          continue;
        }

        if (! selector.IsSegmentSelected(nextSegment)) {
          continue;
        }
        if (setToRemove.find(nextSegment) != setToRemove.end() ) {
          continue;
        }

        q.push(nextSegment);

        setToRemove.insert(nextSegment);
      }
    }

    selector.RemoveTheseSegments(setToRemove);
  }
};
