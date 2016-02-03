#pragma once
#include "precomp.h"
#include "gui/inspectors/segmentation/toolsPage/pageTools.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "gui/widgets/omButton.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegments.h"
#include "segment/omSegment.h"
#include "utility/dataWrappers.h"

namespace om {
namespace segmentationInspector {

class AddSegmentButton : public OmButton<PageTools> {
 public:
  AddSegmentButton(PageTools* d)
      : OmButton<PageTools>(d, "Add Segment", "Add Segment", false) {}

 private:
  void doAction() {
    const SegmentationDataWrapper& sdw = mParent->GetSDW();

    OmSegment* newSeg = sdw.Segments()->AddSegment();

    ElementListBox::RebuildLists(SegmentDataWrapper(sdw, newSeg->value()));

    OmSegmentSelector sel(sdw, this, "addSegmentButton");
    sel.selectJustThisSegment(newSeg->value(), true);
  }
};

}  // namespace segmentationInspector
}  // namespace om
