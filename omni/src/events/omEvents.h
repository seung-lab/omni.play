#ifndef OM_EVENTS_H
#define OM_EVENTS_H

#include "common/omCommon.h"

class OmSelectSegmentsParams;
class SegmentDataWrapper;
class SegmentationDataWrapper;

class OmEvents {
public:
    static void ToolChange();
    static void Redraw2d();
    static void Redraw3d();
    static void SegmentGUIlist(const SegmentationDataWrapper& sdw);
    static void SegmentModified();
    static void SegmentModified(boost::shared_ptr<OmSelectSegmentsParams> params);
    static void ViewCenterChanged();
    static void View3dRecenter();
    static void ViewBoxChanged();
    static void ViewPosChanged();
    static void PreferenceChange(const int key);
};

#endif
