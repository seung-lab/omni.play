#pragma once

/*
 *
 *  Brett Warne - bwarne@mit.edu - 4/21/09
 */
#include "gui/widgets/omSegmentContextMenu.h"
#include "utility/dataWrappers.h"
#include "view3d/omSegmentPickPoint.h"

#include <QtGui>

class OmView3d;
class OmViewGroupState;
class OmMacOSXGestures;

class OmView3dUi {
public:
    OmView3dUi(OmView3d* view3d, OmViewGroupState*);
    ~OmView3dUi();

    //ui events
    bool GestureEvent(QGestureEvent* event);

    void MousePressed(QMouseEvent* event);
    void MouseRelease(QMouseEvent* event);
    void MouseMove(QMouseEvent* event);
    void MouseDoubleClick(QMouseEvent* event);
    void MouseWheel(QWheelEvent* event);
    void KeyPress(QKeyEvent* event);

private:
    OmView3d *const view3d_;
    OmViewGroupState* vgs_;
    boost::scoped_ptr<OmMacOSXGestures> macGestures_;
    OmSegmentContextMenu segmentContextMenu_;
    SegmentDataWrapper prevSDW_;

    void navigationModeMousePressed(QMouseEvent* event);
    void navigationModeMouseRelease(QMouseEvent* event);
    void navigationModeMouseMove(QMouseEvent* event);
    void navigationModeMouseDoubleClick(QMouseEvent* event);
    void navigationModeMouseWheel(QWheelEvent* event);

    void splitModeMouseReleased(QMouseEvent* event);

    void cameraMovementMouseStart(QMouseEvent* event);
    void cameraMovementMouseEnd(QMouseEvent* event);
    void cameraMovementMouseUpdate(QMouseEvent* event);
    void cameraMovementMouseWheel(QWheelEvent* event);

    SegmentDataWrapper pickSegmentMouse(QMouseEvent* event, bool drag);

    void showSegmentContextMenu(QMouseEvent* event);

    void crosshair(QMouseEvent* event);
    void centerAxisOfRotation(QMouseEvent* event);
    OmSegmentPickPoint pickVoxelMouseCrosshair(QMouseEvent* event);
    bool cutSegment(QMouseEvent* event);
    bool annotate(QMouseEvent* event);
    void resetWindow();

    void deselectSegment(const SegmentDataWrapper& sdw);
    void doSelectSegment(const SegmentDataWrapper& sdw, const bool);
};

