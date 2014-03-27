#pragma once
#include "precomp.h"

#include "common/common.h"
#include "events/events.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegments.h"
#include "segment/omSegment.h"
#include "system/cache/omCacheManager.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

class OmSegmentCutActionImpl {
 private:
  SegmentDataWrapper sdw_;
  QVector<om::segment::UserEdge> edges_;
  QString desc;  // TODO: rename to desc_

 public:
  OmSegmentCutActionImpl() {}

  OmSegmentCutActionImpl(const SegmentDataWrapper& sdw)
      : sdw_(sdw), desc("Cutting: ") {}

  void Execute() {
    if (!sdw_.IsValidWrapper()) {
      desc += " failed: invalid segment";
      return;
    }

    OmSegment* seg = sdw_.GetSegment();

    std::vector<om::segment::UserEdge> edges = sdw_.Segments()->CutSegment(seg);

    edges_ = QVector<om::segment::UserEdge>::fromStdVector(edges);

    desc = QString("Cut seg %1; %2 edges").arg(seg->value()).arg(edges.size());

    log_infos << desc.toStdString();

    om::event::SegmentModified();

    sdw_.GetSegmentation()->Segments().SegmentLists().RefreshGUIlists();

    OmCacheManager::TouchFreshness();
    om::event::Redraw2d();
    om::event::Redraw3d();
  }

  void Undo() {
    if (!sdw_.IsValidWrapper()) {
      desc += " undo failed: invalid segment";
      return;
    }

    OmSegment* seg = sdw_.GetSegment();

    std::vector<om::segment::UserEdge> edges = edges_.toStdVector();

    const bool ret = sdw_.Segments()->JoinEdges(edges);

    desc = QString("Uncut seg %1").arg(seg->value());

    if (!ret) {
      om::event::NonFatalEventOccured("Could not fully undo cut");
      desc += " with errors";
    }

    om::event::SegmentModified();

    sdw_.GetSegmentation()->Segments().SegmentLists().RefreshGUIlists();

    OmCacheManager::TouchFreshness();
    om::event::Redraw2d();
    om::event::Redraw3d();
  }

  std::string Description() const { return desc.toStdString(); }

  QString classNameForLogFile() const { return "OmSegmentCutAction"; }

 private:
  template <typename T>
  friend class OmActionLoggerThread;
  friend class QDataStream& operator<<(QDataStream&,
                                       const OmSegmentCutActionImpl&);
  friend class QDataStream& operator>>(QDataStream&, OmSegmentCutActionImpl&);
  friend QTextStream& operator<<(QTextStream& out,
                                 const OmSegmentCutActionImpl&);
};
