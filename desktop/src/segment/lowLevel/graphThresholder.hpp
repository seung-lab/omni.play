#pragma once

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

#include <boost/optional.hpp>

namespace om {
namespace segment {

class GraphThresholder {
 public:
  GraphThresholder(om::segment::Graph& graph, OmValidGroupNum& validGroupNum,
                   OmSegmentLists& segmentList, om::segment::Store& store,
                   om::segment::EdgeVector& edges)
      : graph_(graph),
        validGroupNum_(validGroupNum),
        segmentLists_(segmentList),
        store_(store),
        edges_(edges) {
    joinTaskPool_.Start(&GraphThresholder::updateFromJoin, this, 1);
  }

  void SetGlobalThreshold() {
    log_debugs << om::string::humanizeNum(edges_.size()) << " edges...";

    om::utility::timer timer;

    graph_.SetBatch(true);
    graph_.ClearCache();

    const double stopThreshold =
        OmProject::Globals().Users().UserSettings().getThreshold();

    for (int i = 0; i < edges_.size(); ++i) {
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
      return false;
    }

    if (!validGroupNum_.InSameValidGroup(childRootID, parentID)) {
      return false;
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

  om::segment::Graph& graph_;
  OmValidGroupNum& validGroupNum_;
  OmSegmentLists& segmentLists_;
  om::segment::Store& store_;
  om::segment::EdgeVector& edges_;

  om::thread::ThreadPoolBatched<TaskArgs, GraphThresholder,
                                om::thread::IndivArgPolicy> joinTaskPool_;
};
}
}
