#pragma once

#include "common/common.h"
#include "events/omEvents.h"
#include "project/omProject.h"
#include "segment/lists/omSegmentLists.h"
#include "system/cache/omCacheManager.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

class OmSegmentShatterActionImpl {
 private:
  SegmentDataWrapper sdw_;
  std::vector<OmSegmentEdge> removed_;
  QString desc;

 public:
  OmSegmentShatterActionImpl() {}

  OmSegmentShatterActionImpl(const SegmentDataWrapper& sdw)
      : sdw_(sdw), desc("Shattering: ") {}

  void Execute() {
    removed_ = sdw_.GetSegmentation().Segments()->Shatter(sdw_.GetSegment());

    desc = QString("Shatter seg %1").arg(sdw_.GetSegmentID());
    notify();
  }

  void Undo() {
    FOR_EACH(e, removed_) {
      std::pair<bool, OmSegmentEdge> edge =
          sdw_.GetSegmentation().Segments()->JoinEdge(*e);

      if (!edge.first) {
        std::cout << "edge could not be rejoined...\n";
        return;
      }
    }

    desc = QString("Unshattered %1").arg(sdw_.GetSegmentID());
    notify();
  }

  std::string Description() const { return desc.toStdString(); }

  QString classNameForLogFile() const { return "OmSegmentShatterAction"; }

 private:
  void notify() const {
    std::cout << desc.toStdString() << "\n";

    OmEvents::SegmentModified();

    sdw_.GetSegmentation().SegmentLists()->RefreshGUIlists();

    OmCacheManager::TouchFreshness();
    OmEvents::Redraw2d();
    OmEvents::Redraw3d();
  }

  template <typename T> friend class OmActionLoggerThread;
  friend class QDataStream& operator<<(QDataStream&,
                                       const OmSegmentShatterActionImpl&);
  friend class QDataStream& operator>>(QDataStream&,
                                       OmSegmentShatterActionImpl&);
  friend QTextStream& operator<<(QTextStream& out,
                                 const OmSegmentShatterActionImpl&);
};
