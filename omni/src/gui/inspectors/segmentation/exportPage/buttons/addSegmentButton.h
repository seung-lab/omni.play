#ifndef ADD_SEGMENT_BUTTON_H
#define ADD_SEGMENT_BUTTON_H

#include "gui/inspectors/segmentation/exportPage/pageExport.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "gui/widgets/omButton.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegments.h"
#include "utility/dataWrappers.h"

namespace om {
namespace segmentationInspector {

class AddSegmentButton : public OmButton<PageExport> {
public:
    AddSegmentButton(PageExport* d)
        : OmButton<PageExport>(d,
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

#endif
