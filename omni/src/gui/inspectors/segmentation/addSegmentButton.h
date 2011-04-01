#ifndef ADD_SEGMENT_BUTTON_H
#define ADD_SEGMENT_BUTTON_H

#include "gui/inspectors/segmentation/segmentationInspector.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "gui/widgets/omButton.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegments.h"
#include "utility/dataWrappers.h"

class AddSegmentButton : public OmButton<SegmentationInspector> {
public:
    AddSegmentButton(SegmentationInspector* d)
        : OmButton<SegmentationInspector>(d,
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

#endif
