#pragma once

#include "gui/segmentLists/details/segmentListBase.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "common/common.h"
#include "utility/dataWrappers.h"
#include "segment/lists/omSegmentLists.h"

class SegmentListWorking : public SegmentListBase {
  Q_OBJECT;

 public:
  SegmentListWorking(QWidget* parent, OmViewGroupState& vgs)
      : SegmentListBase(parent, vgs) {}

 private:
  QString getTabTitle() { return QString("Working"); }

  uint64_t Size() {
    return sdw_.SegmentLists()->Size(om::common::SegListType::WORKING);
  }

  std::shared_ptr<GUIPageOfSegments> getPageSegments(
      const GUIPageRequest& request) {
    return sdw_.SegmentLists()->GetSegmentGUIPage(
        om::common::SegListType::WORKING, request);
  }

  int getPreferredTabIndex() { return 0; }

  void makeTabActiveIfContainsJumpedToSegment() {
    ElementListBox::SetActiveTab(this);
  }

  bool shouldSelectedSegmentsBeAddedToRecentList() { return true; }
};
