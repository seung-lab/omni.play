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

 public:
  OmSegmentMultiSplitActionImpl() {}

  OmSegmentMultiSplitActionImpl(const SegmentationDataWrapper& sdw,
                           const std::vector<om::segment::UserEdge>& edges,
                           const om::common::SegIDSet& sources,
                           const om::common::SegIDSet& sinks)
      : edges_(edges), sources_(sources), sinks_(sinks),
        segmentationID_(sdw.GetSegmentationID()),
        desc__("Splitting: ") {}

  void Execute() {
    SegmentationDataWrapper sdw(segmentationID_);

    for (auto edge : edges_) {
      edge = sdw.Segments()->SplitEdge(edge);
    }

    OmJoinSegments joiner(sdw_, sources_);
    joiner.Join();
    OmJoinSegments joiner(sdw_, sinks_);
    joiner.Join();

    sdw.SegmentLists()->RefreshGUIlists();
  }

  void Undo() {
    SegmentationDataWrapper sdw(segmentationID_);

    OmJoinSegments joiner(sdw, sinks_);
    joiner.UnJoin();
    OmJoinSegments joiner(sdw, sources_);
    joiner.UnJoin();

    std::pair<bool, om::segment::UserEdge> userEdge;
    for (auto edge : edges_) {
      edge = sdw.Segments()->JoinEdge(edges);
      if (!edge.childID || !edges.parentID) {
        log_infos << "Can't undo a join that probably failed.";
      }

      if (!edge.first) {
        log_infos << "edge could not be rejoined...";
      }
    }

    sdw.SegmentLists()->RefreshGUIlists();
  }

  std::string desc_ription() const { return desc_.toStdString(); }

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
