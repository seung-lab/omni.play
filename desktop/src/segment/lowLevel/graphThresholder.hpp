#pragma once

#include "common/common.h"
#include "common/string.hpp"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "segment/dataSources.hpp"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lowLevel/children.hpp"
#include "segment/lowLevel/graph.h"
#include "segment/lowLevel/omDynamicForestCache.hpp"
#include "segment/lowLevel/store.h"
#include "threads/threadPoolBatched.hpp"
#include "users/omUsers.h"
#include "utility/timer.hpp"
#include "segment/omSegmentSelector.h"

#include <boost/optional.hpp>


namespace om {
namespace segment {

class GraphThresholder {
 public:
  GraphThresholder(om::segment::Graph& graph, OmValidGroupNum& validGroupNum,
                   OmSegmentLists& segmentList, om::segment::Store& store,
                   om::segment::EdgeVector& edges,
                   std::unordered_map<om::common::SegID, std::vector <Edge*>>& adjacencyMap)
      : graph_(graph),
        validGroupNum_(validGroupNum),
        segmentLists_(segmentList),
        store_(store),
        edges_(edges),
        adjacencyMap_(adjacencyMap) {
    joinTaskPool_.Start(&GraphThresholder::updateFromJoin, this, 1);
  }

  void SetGlobalThreshold() {
    log_debugs << om::string::humanizeNum(edges_.size()) << " edges...";

    om::utility::timer timer;

    graph_.SetBatch(true);
    graph_.ClearCache();

    const double stopThreshold =
        OmProject::Globals().Users().UserSettings().getThreshold();
    std::cout << "Stop threshold currently is: " << stopThreshold << std::endl;

    for (int i = 0; i < edges_.size(); ++i) {
      adjacencyMap_[edges_[i].node1ID].push_back(&edges_[i]);
      adjacencyMap_[edges_[i].node2ID].push_back(&edges_[i]);

      if (1 == edges_[i].userSplit) {
        continue;
      }

      if (edges_[i].threshold >= stopThreshold ||
          1 == edges_[i].userJoin) {  // join
        if (1 == edges_[i].wasJoined) {
          continue;
        }

        if (joinInternalAsync(edges_[i].node2ID, edges_[i].node1ID,
                              edges_[i].threshold, i)) {
          edges_[i].wasJoined = 1;
        } else {
          edges_[i].userSplit = 1;
        }
      }
    }

    joinTaskPool_.JoinPool();

    graph_.SetBatch(false);

    timer.PrintDone();
  }

  void ResetGlobalThreshold() {
    log_debugs << string::humanizeNum(edges_.size()) << " edges...";

    utility::timer timer;

    graph_.SetBatch(true);
    graph_.ClearCache();

    const double stopThreshold =
        OmProject::Globals().Users().UserSettings().getThreshold();
    std::cout << "Stop threshold currently is: " << stopThreshold << std::endl;

    for (int i = 0; i < edges_.size(); ++i) {
      if (1 == edges_[i].userSplit) {
        continue;
      }

      if (edges_[i].threshold >= stopThreshold ||
          1 == edges_[i].userJoin) {  // join
        if (1 == edges_[i].wasJoined) {
          continue;
        }

        if (joinInternal(edges_[i].node2ID, edges_[i].node1ID,
                         edges_[i].threshold, i)) {
          edges_[i].wasJoined = 1;
        } else {
          edges_[i].userSplit = 1;
        }
      } else {  // split
        if (0 == edges_[i].wasJoined) {
          continue;
        }
        if (splitChildFromParentInternal(edges_[i].node1ID)) {
          edges_[i].wasJoined = 0;
        } else {
          edges_[i].userJoin = 1;
        }
      }
    }

    graph_.SetBatch(false);

    timer.PrintDone();
  }

  void AddSegments_BreadthFirstSearch(OmSegmentSelector* sel, om::common::SegID SegmentID) {
    std::queue <om::common::SegID> q;
    Edge *currEdge;
    om::common::SegID currSegment, nextSegment;
    double threshold = OmProject::Globals().Users().UserSettings().getASThreshold();

    q.push(SegmentID);

    om::common::SegIDList vecToAdd;
    om::common::SegIDSet setToAdd;
    setToAdd.insert(SegmentID);
    vecToAdd.push_back(SegmentID);

    int br=0;
    while (!q.empty()) {
      br++;
      currSegment = q.front();

      if (br == 1000) {
          break;
      }

      q.pop();

      for ( int i = 0; i < adjacencyMap_[currSegment].size(); i++ ) {
        currEdge = adjacencyMap_[currSegment][i];

        if (currSegment == currEdge->node2ID) {
          nextSegment = currEdge->node1ID;
        } else {
          nextSegment = currEdge->node2ID;
        }

        log_debugs << "BFS for (" << currSegment << ") looking at: " << nextSegment <<
                      " (" << currEdge->threshold << ") <? (" << threshold << ")";

        // TODO::reorderLog move before log
        if (currEdge->threshold < threshold) {
          continue;
        }

        if (sel->IsSegmentSelected(nextSegment)) {
          continue;
        }
        if (setToAdd.find(nextSegment) != setToAdd.end()) {
          continue;
        }

        q.push(nextSegment);
        setToAdd.insert(nextSegment);
        vecToAdd.push_back(nextSegment);
      }
    }
    sel->InsertSegments(vecToAdd);
  }

  void Trim(OmSegmentSelector* sel, om::common::SegID SegmentID) {
      Edge *currEdge;
      om::common::SegID currSegment, nextSegment;

      uint32_t mini = -1;

      if (!sel->GetOrderOfAdding(SegmentID)) {
        for (int i = 0; i < adjacencyMap_[SegmentID].size(); i++) {
          currEdge = adjacencyMap_[SegmentID][i];

          if (SegmentID == currEdge->node2ID) {
            nextSegment = currEdge->node1ID;
          } else {
            nextSegment = currEdge->node2ID;
          }

          if (sel->GetOrderOfAdding(nextSegment) &&
              (mini == -1 || sel->GetOrderOfAdding(nextSegment) < mini)) {
            mini = sel->GetOrderOfAdding(nextSegment);
          }
        }
      } else {
        mini = sel->GetOrderOfAdding(SegmentID);
      }
      std::cout << "Trying to trim " << SegmentID << " mini is" << mini << std::endl;

      std::queue <om::common::SegID> q;
      q.push(SegmentID);

      om::common::SegIDSet setToRemove;

      while (!q.empty()) {
        currSegment = q.front();

        q.pop();

        for (int i = 0; i < adjacencyMap_[currSegment].size(); i++) {
          currEdge = adjacencyMap_[currSegment][i];

          if (currSegment == currEdge->node2ID) {
            nextSegment = currEdge->node1ID;
          } else {
            nextSegment = currEdge->node2ID;
          }

          log_debugs << "TRIM for (" << nextSegment << ") looking at: " <<
                      currEdge->node1ID << " - " << currEdge->node2ID <<
                      " (" << sel->GetOrderOfAdding(nextSegment) << ") <=? mini (" <<
                        mini << ")";

          // TODO::reorderLog move before log
          if (sel->GetOrderOfAdding(nextSegment) <= mini) {
            continue;
          }

          if (! sel->IsSegmentSelected(nextSegment)) {
            continue;
          }
          if (setToRemove.find(nextSegment) != setToRemove.end() ) {
            continue;
          }

          q.push(nextSegment);

          setToRemove.insert(nextSegment);
        }
      }

      sel->RemoveTheseSegments(setToRemove);
  }

  void AddSegments_BFS_DynamicThreshold(OmSegmentSelector* sel, om::common::SegID SegmentID) {
    std::queue <om::common::SegID> q;
    Edge *currEdge;
    om::common::SegID currSegment, nextSegment;
    double difference,top;

    q.push(SegmentID);
    om::common::SegIDList vecToAdd;
    om::common::SegIDSet setToAdd;
    setToAdd.insert(SegmentID);
    vecToAdd.push_back(SegmentID);

    int br=0,sizeList;
    while (!q.empty()) {
      br++;

      currSegment = q.front();

      if (br == 1000) {
        break;
      }

      q.pop();

      sizeList = adjacencyMap_[currSegment].size();
      difference = adjacencyMap_[currSegment][0]->threshold -
          adjacencyMap_[currSegment][sizeList - 1]->threshold;
      if (sizeList == 1) {
        top = 0;
      }

      for (int i = 0; i < sizeList; i++) {
        currEdge = adjacencyMap_[currSegment][i];
        if (currSegment == currEdge->node2ID) {
            nextSegment = currEdge->node1ID;
        } else {
          nextSegment = currEdge->node2ID;
        }

        log_debugs << "Dynamic BFS for (" << currSegment << ") looking at: " << nextSegment <<
                      " (" << adjacencyMap_[currSegment][0]->threshold <<
                      ") <-.05> (" << adjacencyMap_[currSegment][i]->threshold << ")";

        // TODO::reorderLog move before log
        if (adjacencyMap_[currSegment][0]->threshold - adjacencyMap_[currSegment][i]->threshold > 0.05) {
          break;
        }

        if (sel->IsSegmentSelected(nextSegment)) {
          continue;
        }

        if (setToAdd.find(nextSegment) != setToAdd.end()) {
          continue;
        }

        q.push(nextSegment);
        setToAdd.insert(nextSegment);
        vecToAdd.push_back(nextSegment);
      }
    }

    sel->InsertSegments(vecToAdd);
  }

 private:
  struct TaskArgs {
    om::common::SegID childRootID;
    om::common::SegID parentID;
    om::common::SegID parentRootID;
    double threshold;
    int edgeNumber;
  };

  inline om::common::SegID Root(const om::common::SegID segID) {
    return graph_.Root(segID);
  }

  bool sizeCheck(const om::common::SegID a, const om::common::SegID b,
                 const double threshold) {
    return threshold == 0 ||
           ((segmentLists_.GetSizeWithChildren(Root(a)) +
             segmentLists_.GetSizeWithChildren(Root(b))) < threshold);
  }

  boost::optional<TaskArgs> doJoin(const om::common::SegID parentID,
                                   const om::common::SegID childUnknownDepthID,
                                   const double threshold,
                                   const int edgeNumber) {
    const om::common::SegID childRootID = Root(childUnknownDepthID);
    const om::common::SegID parentRootID = Root(parentID);

    if (childRootID == parentRootID) {
      return boost::optional<TaskArgs>();
    }

    if (!validGroupNum_.InSameValidGroup(childRootID, parentID)) {
      return boost::optional<TaskArgs>();
    }

    graph_.Join(childRootID, parentID);
    return TaskArgs{childRootID, parentID, parentRootID, threshold, edgeNumber};
  }

  void updateFromJoin(const TaskArgs& t) {
    OmSegment* childRoot = store_.GetSegment(t.childRootID);
    OmSegment* parent = store_.GetSegment(t.parentID);

    graph_.SegChildren().AddChild(t.parentID, childRoot);
    childRoot->setParent(parent, t.threshold);
    childRoot->setEdgeNumber(t.edgeNumber);

    OmSegment* parentRoot = parent;
    if (t.parentRootID != t.parentID) {
      parentRoot = store_.GetSegment(t.parentRootID);
    }

    parentRoot->touchFreshnessForMeshes();
    segmentLists_.UpdateSizeListsFromJoin(parentRoot, childRoot);
  }

  bool joinInternalAsync(const om::common::SegID parentID,
                         const om::common::SegID childUnknownDepthID,
                         const double threshold, const int edgeNumber) {
    auto ta = doJoin(parentID, childUnknownDepthID, threshold, edgeNumber);
    if (ta) {
      joinTaskPool_.AddOrSpawnTasks(ta.get());
    }
    return ta;
  }

  bool joinInternal(const om::common::SegID parentID,
                    const om::common::SegID childUnknownDepthID,
                    const double threshold, const int edgeNumber) {
    auto ta = doJoin(parentID, childUnknownDepthID, threshold, edgeNumber);
    if (ta) {
      updateFromJoin(ta.get());
    }
    return ta;
  }

  bool splitChildFromParentInternal(const om::common::SegID childID) {
    OmSegment* child = store_.GetSegment(childID);

    if (child->getThreshold() > 1) {
      return false;
    }

    OmSegment* parent = child->getParent();
    if (!parent) {  // user manually split?
      return false;
    }

    if (child->IsValidListType() || parent->IsValidListType()) {
      return false;
    }

    graph_.SegChildren().RemoveChild(parent, child);
    graph_.Cut(childID);
    child->setParent(nullptr);  // TODO: also set threshold??
    child->setEdgeNumber(-1);

    OmSegment* parentRoot = store_.GetSegment(graph_.Root(parent->value()));

    parentRoot->touchFreshnessForMeshes();
    child->touchFreshnessForMeshes();

    const SizeAndNumPieces childInfo =
        graph_.SegChildren().ComputeSegmentSizeWithChildren(child);
    segmentLists_.UpdateSizeListsFromSplit(parentRoot, child, childInfo);

    return true;
  }

  double SizeOfBFSGrowth(om::common::SegID SegmentID, double threshold) {
    double totalSize = 0;

    std::queue <om::common::SegID> q;
    Edge *currEdge;
    om::common::SegID currSegment, nextSegment;

    q.push(SegmentID);
    totalSize += segmentLists_.GetSizeWithChildren(Root(SegmentID)); // PROBABLY SHOULD BE JUST THE CURRENT SEGMENT WITHOUT THE CHILDREN

    std::unordered_map <om::common::SegID,bool> used;
    used[SegmentID] = 1;

    while (!q.empty()) {
      currSegment = q.front();

      q.pop();

      for (int i = 0; i < adjacencyMap_[currSegment].size(); i++) {
        currEdge = adjacencyMap_[currSegment][i];

        if (currEdge->threshold < threshold) {
          continue;
        }

        if (currSegment == currEdge->node2ID) {
          nextSegment = currEdge->node1ID;
        } else {
          nextSegment = currEdge->node2ID;
        }

        if (used[nextSegment]) {
          continue;
        }

        q.push(nextSegment);
        used[nextSegment] = 1;
        totalSize += segmentLists_.GetSizeWithChildren(Root(nextSegment)); // PROBABLY SHOULD BE JUST THE CURRENT SEGMENT WITHOUT THE CHILDREN
      }
    }

    return totalSize;
  }

  om::segment::Graph& graph_;
  OmValidGroupNum& validGroupNum_;
  OmSegmentLists& segmentLists_;
  om::segment::Store& store_;
  om::segment::EdgeVector& edges_;
  std::unordered_map<om::common::SegID, std::vector <Edge*>>& adjacencyMap_;
  some_type distribution_;
  std::unordered_map<om::common::SegID,OmSegment*> accessToSegments_;

  om::thread::ThreadPoolBatched<TaskArgs, GraphThresholder,
                                om::thread::IndivArgPolicy> joinTaskPool_;
};
} // namespace segment
} // namespace om
