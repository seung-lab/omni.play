#pragma once

/*
 *
 *  Brett Warne - bwarne@mit.edu - 4/21/09
 */
#include "gui/widgets/omSegmentContextMenu.h"
#include "utility/dataWrappers.h"

#include <QtGui>

class OmView3d;
class OmViewGroupState;
class OmMacOSXGestures;
class OmSegmentPickPoint;

class OmView3dUi {
public:
    OmView3dUi(OmView3d* view3d, OmViewGroupState* );
    ~OmView3dUi();

    //ui events
    bool gestureEvent(QGestureEvent *event);

    void MousePressed(QMouseEvent *event);
    void MouseRelease(QMouseEvent *event);
    void MouseMove(QMouseEvent *event);
    void MouseDoubleClick(QMouseEvent *event);
    void MouseWheel(QWheelEvent *event);
    void KeyPress(QKeyEvent *event);

    //navigation mode
    void NavigationModeMousePressed(QMouseEvent *event);
    void NavigationModeMouseRelease(QMouseEvent *event);
    void NavigationModeMouseMove(QMouseEvent *event);
    void NavigationModeMouseDoubleClick(QMouseEvent *event);
    void NavigationModeMouseWheel(QWheelEvent *event);

    //dend mode
    void DendModeMouseReleased(QMouseEvent *event);

    //camera movement
    void CameraMovementMouseStart(QMouseEvent *event);
    void CameraMovementMouseEnd(QMouseEvent *event);
    void CameraMovementMouseUpdate(QMouseEvent *event);
    void CameraMovementMouseWheel(QWheelEvent *event);

    //segment picking
    SegmentDataWrapper PickSegmentMouse(QMouseEvent *event, bool drag);

    //segment actions
    void SegmentSelectToggleMouse(QMouseEvent *event, bool drag);

    //segment context menu
    void ShowSegmentContextMenu(QMouseEvent *event);

private:
    OmView3d *const view3d_;
    OmViewGroupState* vgs_;

    boost::scoped_ptr<OmMacOSXGestures> macGestures_;

    OmSegmentContextMenu segmentContextMenu_;

    void crosshair(QMouseEvent* event);
    void CenterAxisOfRotation(QMouseEvent* event);
    OmSegmentPickPoint PickVoxelMouseCrosshair(QMouseEvent* event);
    void CutModeMouseReleased(QMouseEvent* event);

    SegmentDataWrapper prevSDW_;
    void resetWindow();

    bool cPressed_;

    void doSelectSegment(const SegmentDataWrapper& sdw, const bool);
};

