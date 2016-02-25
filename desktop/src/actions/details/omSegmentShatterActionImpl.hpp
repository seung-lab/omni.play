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
#include "volume/omSegmentation.h"

class OmSegmentShatterActionImpl {
 private:
  SegmentDataWrapper sdw_;
  std::vector<om::segment::UserEdge> removed_;
  QString desc;

 public:
  OmSegmentShatterActionImpl() {}

  OmSegmentShatterActionImpl(const SegmentDataWrapper& sdw)
      : sdw_(sdw), desc("Shattering: ") {}

  void Execute() {
    if (!sdw_.IsValidWrapper()) {
      return;
    }

    removed_ = sdw_.GetSegmentation()->Segments().Shatter(sdw_.GetSegment());

    desc = QString("Shatter seg %1").arg(sdw_.GetSegmentID());

    sdw_.GetSegmentation()->Segments().SegmentLists().RefreshGUIlists();
  }

  void Undo() {
    FOR_EACH(e, removed_) {
      std::pair<bool, om::segment::UserEdge> edge =
          sdw_.GetSegmentation()->Segments().JoinEdge(*e);


      if (!edge.first) {
        log_infos << "edge could not be rejoined...";
        return;
      }
    }

    desc = QString("Unshattered %1").arg(sdw_.GetSegmentID());

    sdw_.GetSegmentation()->Segments().SegmentLists().RefreshGUIlists();
  }

  std::string Description() const { return desc.toStdString(); }

  QString classNameForLogFile() const { return "OmSegmentShatterAction"; }

 private:
  template <typename T>
  friend class OmActionLoggerThread;
  friend class QDataStream& operator<<(QDataStream&,
                                       const OmSegmentShatterActionImpl&);
  friend class QDataStream& operator>>(QDataStream&,
                                       OmSegmentShatterActionImpl&);
  friend QTextStream& operator<<(QTextStream& out,
                                 const OmSegmentShatterActionImpl&);
};
