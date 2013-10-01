#pragma once

#include "common/common.h"
#include "events/events.h"
#include "project/omProject.h"
#include "segment/lists/omSegmentLists.h"
#include "system/cache/omCacheManager.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

class OmSegmentSplitActionImpl {
 private:
  OmSegmentEdge mEdge;
  om::common::ID mSegmentationID;
  QString desc;

 public:
  OmSegmentSplitActionImpl() {}

  OmSegmentSplitActionImpl(const SegmentationDataWrapper& sdw,
                           const OmSegmentEdge& edge)
      : mEdge(edge),
        mSegmentationID(sdw.GetSegmentationID()),
        desc("Splitting: ") {}

  void Execute() {
    SegmentationDataWrapper sdw(mSegmentationID);

    mEdge = sdw.Segments()->SplitEdge(mEdge);

    desc =
        QString("Split seg %1 from %2").arg(mEdge.childID).arg(mEdge.parentID);

    std::cout << desc.toStdString() << "\n";

    om::event::SegmentModified();

    sdw.SegmentLists()->RefreshGUIlists();

    OmCacheManager::TouchFreshness();
    om::event::Redraw2d();
    om::event::Redraw3d();
  }

  void Undo() {
    SegmentationDataWrapper sdw(mSegmentationID);

    std::pair<bool, OmSegmentEdge> edge = sdw.Segments()->JoinEdge(mEdge);

    if (!mEdge.childID || !mEdge.parentID) {
      printf("Can't undo a join that probably failed.\n");
      return;
    }

    if (!edge.first) {
      std::cout << "edge could not be rejoined...\n";
      return;
    }

    mEdge = edge.second;

    desc =
        QString("Joined seg %1 to %2").arg(mEdge.childID).arg(mEdge.parentID);

    om::event::SegmentModified();

    sdw.SegmentLists()->RefreshGUIlists();

    OmCacheManager::TouchFreshness();
    om::event::Redraw2d();
    om::event::Redraw3d();
  }

  std::string Description() const { return desc.toStdString(); }

  QString classNameForLogFile() const { return "OmSegmentSplitAction"; }

 private:
  template <typename T> friend class OmActionLoggerThread;
  friend class QDataStream& operator<<(QDataStream&,
                                       const OmSegmentSplitActionImpl&);
  friend class QDataStream& operator>>(QDataStream&, OmSegmentSplitActionImpl&);
  friend QTextStream& operator<<(QTextStream& out,
                                 const OmSegmentSplitActionImpl&);
};
