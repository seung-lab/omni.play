#ifndef SEG_INSPECTOR_BUTTONS_HPP
#define SEG_INSPECTOR_BUTTONS_HPP

#include "headless/headlessImpl.hpp"
#include "gui/widgets/omButton.hpp"
#include "gui/inspectors/segmentation/segmentationInspector.h"

class RebuildCenterOfSegmentDataButton : public OmButton<SegmentationInspector> {
public:
    RebuildCenterOfSegmentDataButton(SegmentationInspector * d)
        : OmButton<SegmentationInspector>( d,
                                  "Rebuild segment center data",
                                  "Rebuild",
                                  false)
    {}

private:
    void doAction(){
        HeadlessImpl::RebuildCenterOfSegmentData(1);
    }
};


class SegmentationInspectorButtons {
private:

public:

};

#endif
