#pragma once
#include "precomp.h"

#include "common/common.h"
#include "events/events.h"
#include "project/omProject.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegments.h"
#include "system/cache/omCacheManager.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

class OmSegmentMultiSplitActionImpl {
 private:
  std::vector<om::segment::UserEdge> edges_;
  om::common::SegIDSet sources_;
  om::common::SegIDSet sinks_;
  om::common::ID segmentationID_;
  QString desc_;

  /*
   * Return only the seg ids that have different roots (means that they
   * should be joined!)
   */
  om::common::SegIDSet getNeedsJoin(om::common::SegIDSet segIDs, 
      const SegmentationDataWrapper& segmentationDataWrapper) {
    om::common::SegIDSet newSegIDs;

    if (segIDs.empty()) {
      return newSegIDs;
    }

    om::common::SegID rootID =
      segmentationDataWrapper.Segments()->FindRootID(*segIDs.begin());
    for (auto segID : segIDs) {
      if (segmentationDataWrapper.Segments()->FindRootID(segID) != rootID) {
        newSegIDs.insert(segID);
      }
    }
    return newSegIDs;
  }

 public:
  OmSegmentMultiSplitActionImpl() {}

  OmSegmentMultiSplitActionImpl(const SegmentationDataWrapper& sdw,
                           const std::vector<om::segment::UserEdge>& edges,
                           const om::common::SegIDSet& sources,
                           const om::common::SegIDSet& sinks)
      : edges_(edges), sources_(sources), sinks_(sinks),
        segmentationID_(sdw.GetSegmentationID()),
        desc_("Splitting: ") {}

  void Execute() {
    SegmentationDataWrapper sdw(segmentationID_);

    for (auto edge : edges_) {
      std::cout << "Trying to cut " << edge.parentID << "-" << edge.childID << std::endl;
      auto returnEdge = sdw.Segments()->SplitEdge(edge);
      std::cout << "got back:" << returnEdge.parentID << "-" << returnEdge.childID << std::endl;
    }

    // update the source/sink list to join only those who are now different segments
    sources_ = getNeedsJoin(sources_, sdw);
    sinks_ = getNeedsJoin(sinks_, sdw);

    OmJoinSegments sourceJoiner(sdw, sources_);
    sourceJoiner.Join();
    OmJoinSegments sinkJoiner(sdw, sinks_);
    sinkJoiner.Join();

    sdw.SegmentLists()->RefreshGUIlists();
  }

  void Undo() {
    SegmentationDataWrapper sdw(segmentationID_);

    OmJoinSegments sourceJoiner(sdw, sinks_);
    sourceJoiner.UnJoin();
    OmJoinSegments sinkJoiner(sdw, sources_);
    sinkJoiner.UnJoin();

    std::pair<bool, om::segment::UserEdge> returnEdgeTuple;
    for (auto edge : edges_) {
      std::cout << "Trying to join " << edge.parentID << "-" << edge.childID << std::endl;
      returnEdgeTuple = sdw.Segments()->JoinEdge(edge);
      std::cout << "got back: " << returnEdgeTuple.first << " : " << returnEdgeTuple.second.parentID << "-" << returnEdgeTuple.second.childID << std::endl;
      // todo fix this!
      if (!edge.childID || !edge.parentID) {
        log_infos << "Can't undo a join that probably failed.";
      }

      if (!returnEdgeTuple.first) {
        log_infos << "edge could not be rejoined...";
        return;
      }
    }

    sdw.SegmentLists()->RefreshGUIlists();
  }

  std::string Description() const { return desc_.toStdString(); }

  QString classNameForLogFile() const { return "OmSegmentSplitAction"; }

 private:
  template <typename T>
  friend class OmActionLoggerThread;
  friend class QDataStream& operator<<(QDataStream&,
                                       const OmSegmentMultiSplitActionImpl&);
  friend class QDataStream& operator>>(QDataStream&, OmSegmentMultiSplitActionImpl&);
  friend QTextStream& operator<<(QTextStream& out,
                                 const OmSegmentMultiSplitActionImpl&);
};
