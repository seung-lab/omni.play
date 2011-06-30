#pragma once
#include "gui/inspectors/segmentation/toolsPage/pageTools.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "gui/widgets/omButton.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegments.h"
#include "utility/dataWrappers.h"


namespace om {
namespace segmentationInspector {

class AddSegmentButton : public OmButton<PageTools> {
public:
    AddSegmentButton(PageTools* d)
        : OmButton<PageTools>(d,
                                 "Add Segment",
                                 "Add Segment",
                                 false)
    {}

private:
    void doAction()
    {
        const SegmentationDataWrapper& sdw = mParent->GetSDW();

        OmSegment* newSeg = sdw.Segments()->AddSegment();

        ElementListBox::RebuildLists(SegmentDataWrapper(newSeg));

        OmSegmentSelector sel(sdw, this, "addSegmentButton" );
        sel.selectJustThisSegment(newSeg->value(), true);
        sel.sendEvent();
    }
};

} // namespace segmentationInspector
} // namespace om

