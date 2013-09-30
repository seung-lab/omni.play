#pragma once

#include "common/common.h"
#include "gui/segmentLists/details/segmentListBase.h"
#include "segment/lists/omSegmentLists.h"
#include "utility/dataWrappers.h"

class SegmentListRecent : public SegmentListBase {
  Q_OBJECT public : SegmentListRecent(QWidget* parent, OmViewGroupState* vgs)
                    : SegmentListBase(parent, vgs) {}

 private:
  QString getTabTitle() { return QString("Recent"); }

  uint64_t Size() { return sdw_.SegmentLists()->SizeRecent(); }

  std::shared_ptr<GUIPageOfSegments> getPageSegments(
      const GUIPageRequest& request) {
    return sdw_.SegmentLists()->GetSegmentGUIPageRecent(request);
  }

  int getPreferredTabIndex() { return 3; }

  void makeTabActiveIfContainsJumpedToSegment() {
    // don't jump to this tab
  }

  bool shouldSelectedSegmentsBeAddedToRecentList() { return false; }
};
