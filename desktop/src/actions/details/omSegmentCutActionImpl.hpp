#pragma once

#include "common/common.h"
#include "events/omEvents.h"
#include "segment/lists/omSegmentLists.h"
#include "system/cache/omCacheManager.h"
#include "utility/dataWrappers.h"

#include <QVector>

class OmSegmentCutActionImpl {
 private:
  SegmentDataWrapper sdw_;
  QVector<OmSegmentEdge> edges_;
  QString desc;  //TODO: rename to desc_

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

    std::vector<OmSegmentEdge> edges = sdw_.Segments()->CutSegment(seg);

    edges_ = QVector<OmSegmentEdge>::fromStdVector(edges);

    desc = QString("Cut seg %1; %2 edges").arg(seg->value()).arg(edges.size());

    std::cout << desc.toStdString() << "\n";

    OmEvents::SegmentModified();

    sdw_.GetSegmentation().SegmentLists()->RefreshGUIlists();

    OmCacheManager::TouchFreshness();
    OmEvents::Redraw2d();
    OmEvents::Redraw3d();
  }

  void Undo() {
    if (!sdw_.IsValidWrapper()) {
      desc += " undo failed: invalid segment";
      return;
    }

    OmSegment* seg = sdw_.GetSegment();

    std::vector<OmSegmentEdge> edges = edges_.toStdVector();

    const bool ret = sdw_.Segments()->JoinEdges(edges);

    desc = QString("Uncut seg %1").arg(seg->value());

    if (!ret) {
      OmEvents::NonFatalEvent("Could not fully undo cut");
      desc += " with errors";
    }

    OmEvents::SegmentModified();

    sdw_.GetSegmentation().SegmentLists()->RefreshGUIlists();

    OmCacheManager::TouchFreshness();
    OmEvents::Redraw2d();
    OmEvents::Redraw3d();
  }

  std::string Description() const { return desc.toStdString(); }

  QString classNameForLogFile() const { return "OmSegmentCutAction"; }

 private:
  template <typename T> friend class OmActionLoggerThread;
  friend class QDataStream& operator<<(QDataStream&,
                                       const OmSegmentCutActionImpl&);
  friend class QDataStream& operator>>(QDataStream&, OmSegmentCutActionImpl&);
  friend QTextStream& operator<<(QTextStream& out,
                                 const OmSegmentCutActionImpl&);
};
