#pragma once
#include "precomp.h"
#include "gui/inspectors/segmentation/toolsPage/pageTools.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "gui/widgets/segmentationDataWrapperButton.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegments.h"
#include "segment/omSegment.h"
#include "utility/dataWrappers.h"

namespace om {
namespace segmentationInspector {

class AddSegmentButton : public SegmentationDataWrapperButton {
 public:
  AddSegmentButton(QWidget* widget,
      const SegmentationDataWrapper& segmentationDataWrapper)
      : SegmentationDataWrapperButton(widget, "Add Segment",
          "Add Segment", false, segmentationDataWrapper) {}

 private:
  void onLeftClick() override {
    const SegmentationDataWrapper& sdw = GetSegmentationDataWrapper();

    OmSegment* newSeg = sdw.Segments()->AddSegment();

    ElementListBox::RebuildLists(SegmentDataWrapper(sdw, newSeg->value()));

    OmSegmentSelector sel(sdw, this, "addSegmentButton");
    sel.selectJustThisSegment(newSeg->value(), true);
  }
};

}  // namespace segmentationInspector
}  // namespace om
