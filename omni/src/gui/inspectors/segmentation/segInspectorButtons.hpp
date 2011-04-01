#ifndef SEG_INSPECTOR_BUTTONS_HPP
#define SEG_INSPECTOR_BUTTONS_HPP

#include "headless/headlessImpl.hpp"
#include "gui/widgets/omButton.hpp"
#include "gui/inspectors/segmentation/segInspector.h"

class RebuildCenterOfSegmentDataButton : public OmButton<SegInspector> {
public:
    RebuildCenterOfSegmentDataButton(SegInspector * d)
        : OmButton<SegInspector>( d,
                                  "Rebuild segment center data",
                                  "Rebuild",
                                  false)
    {}

private:
    void doAction(){
        HeadlessImpl::RebuildCenterOfSegmentData(1);
    }
};


class SegInspectorButtons {
private:

public:

};

#endif
