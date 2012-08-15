#pragma once

#include "common/omCommon.h"

class OmSelectSegmentsParams;
class SegmentDataWrapper;
class SegmentationDataWrapper;

class OmEvents {
public:
    static void ToolChange();
    static void Redraw2d();
    static void Redraw2dBlocking();
    static void Redraw3d();
    static void SegmentGUIlist(const SegmentationDataWrapper& sdw,
                               const bool stayOnPage);
    static void SegmentModified();
    static void SegmentModified(om::shared_ptr<OmSelectSegmentsParams> params);
    static void SegmentSelected();
    static void ViewCenterChanged();
    static void View3dRecenter();
    static void ViewBoxChanged();
    static void ViewPosChanged();
    static void AbsOffsetChanged();
    static void PreferenceChange(const int key);

    static void UpdateSegmentPropBox(QWidget* widget, const QString& title);

    // used to update GUI spin box value from do/undo action
    static void RefreshMSTthreshold();

    static void NonFatalEvent(const QString err);

    static void AnnotationEvent();
};

