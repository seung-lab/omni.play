#include "actions/omActions.h"
#include "events/omEvents.h"
#include "gui/sidebars/right/dendToolbar.h"
#include "mesh/omDrawOptions.h"
#include "project/omProject.h"
#include "segment/omSegmentCenter.hpp"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegmentUtils.hpp"
#include "segment/omSegments.h"
#include "system/omStateManager.h"
#include "view3d/omCamera.h"
#include "view3d/omMacOSXgestures.hpp"
#include "view3d/omSegmentPickPoint.h"
#include "view3d/omView3d.h"
#include "view3d/omView3dUi.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"

OmView3dUi::OmView3dUi(OmView3d* view3d, OmViewGroupState* vgs)
    : view3d_(view3d)
    , vgs_(vgs)
    , macGestures_(new OmMacOSXGestures(view3d))
{}

OmView3dUi::~OmView3dUi()
{}

void OmView3dUi::MousePressed(QMouseEvent* event){
    NavigationModeMousePressed(event);
}

void OmView3dUi::MouseRelease(QMouseEvent* event)
{
    if(om::tool::SPLIT == OmStateManager::GetToolMode())
    {
        DendModeMouseReleased(event);
        return;

    } else if(om::tool::CUT == OmStateManager::GetToolMode()){
        cutModeMouseReleased(event);
        return;
    }

    NavigationModeMouseRelease(event);
}

void OmView3dUi::MouseMove(QMouseEvent* event)
{
    //ignore movement without buttons
    if(!event->buttons())
        return;

    NavigationModeMouseMove(event);
}

void OmView3dUi::MouseDoubleClick(QMouseEvent* event)
{
    NavigationModeMouseDoubleClick(event);
}

void OmView3dUi::MouseWheel(QWheelEvent* event)
{
    NavigationModeMouseWheel(event);
}

void OmView3dUi::KeyPress(QKeyEvent* event)
{
    switch(event->key()){
    case Qt::Key_C:
        OmSegmentCenter::CenterSegment(vgs_);
        break;
    case Qt::Key_Escape:
        resetWindow();
        break;
    case Qt::Key_Minus:
        view3d_->DoZoom(-1);
        break;
    case Qt::Key_Equal:
        view3d_->DoZoom(1);
        break;
    }
}

void OmView3dUi::DendModeMouseReleased(QMouseEvent* event)
{
    const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);

    view3d_->updateGL();

    if(!pickPoint.sdw.IsSegmentValid()) {
        return;
    }

    OmActions::FindAndSplitSegments(pickPoint.sdw, vgs_, pickPoint.voxel);
}

void OmView3dUi::cutModeMouseReleased(QMouseEvent* event)
{
    const SegmentDataWrapper sdw = PickSegmentMouse(event, false);

    if(!sdw.IsSegmentValid())
    {
        view3d_->updateGL();
        return;
    }

    view3d_->updateGL();
    OmActions::FindAndCutSegments(sdw, vgs_);
}

/////////////////////////////////
///////          Navigation Mode Methods

void OmView3dUi::NavigationModeMousePressed(QMouseEvent* event)
{
    const bool rightMouseButton = event->buttons() & Qt::RightButton;
    const bool noModifiers = !event->modifiers();
    const bool controlModifier = event->modifiers() & Qt::ControlModifier;
    const bool altModifier = event->modifiers() & Qt::AltModifier;

    if(rightMouseButton)
    {
        if(noModifiers){
            ShowSegmentContextMenu(event);

        } else if(controlModifier)
        {
            const bool shiftModifier = event->modifiers() & Qt::ShiftModifier;
            const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);
            doSelectSegment(pickPoint.sdw, shiftModifier);

        } else {
            CameraMovementMouseStart(event);
        }

        return;
    }

    if(altModifier)
    {
        const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);
        deselectSegment(pickPoint.sdw);

    } else if(controlModifier){
        crosshair(event);

    } else {
        CameraMovementMouseStart(event);
    }
}

void OmView3dUi::deselectSegment(const SegmentDataWrapper& sdw)
{
    if(!sdw.IsSegmentValid()){
        return;
    }

    const OmID segmentID = sdw.getID();

    OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this, "view3d");
    sel.augmentSelectedSet(segmentID, false);

    sel.sendEvent();
}

void OmView3dUi::doSelectSegment(const SegmentDataWrapper& sdw, const bool augment_selection)
{
    if(!sdw.IsSegmentValid()){
        return;
    }

    const OmID segmentID = sdw.getID();

    OmSegmentSelected::Set(sdw);

    OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this, "view3d");

    if(augment_selection){
        sel.augmentSelectedSet_toggle(segmentID);

    } else {
        sel.selectJustThisSegment_toggle(segmentID);
    }

    sel.sendEvent();
}

void OmView3dUi::NavigationModeMouseRelease(QMouseEvent* event){
    CameraMovementMouseEnd(event);
}

void OmView3dUi::NavigationModeMouseMove(QMouseEvent* event){
    CameraMovementMouseUpdate(event);
}

void OmView3dUi::NavigationModeMouseDoubleClick(QMouseEvent* event){
    centerAxisOfRotation(event);
}

void OmView3dUi::NavigationModeMouseWheel(QWheelEvent* event){
    CameraMovementMouseWheel(event);
}

/////////////////////////////////
///////          Camera Movement Methods

void OmView3dUi::CameraMovementMouseStart(QMouseEvent* event)
{
    //get point and modifier
    Vector2f point = Vector2f(event->x(), event->y());
    bool shift_modifier = event->modifiers() & Qt::ShiftModifier;

    //left w/o shift moves rotate
    if(event->buttons() & Qt::LeftButton && !shift_modifier) {
        view3d_->mCamera.MovementStart(CAMERA_ORBIT_ROTATE, point);

        //left + shift moves lookat
    } else if(event->buttons() & Qt::LeftButton && shift_modifier) {
        view3d_->mCamera.MovementStart(CAMERA_LOOKAT_ROTATE, point);

        //mid button pans
    } else if(event->buttons() & Qt::MidButton) {
        view3d_->mCamera.MovementStart(CAMERA_PAN, point);

        //right button zooms
    } else if(event->buttons() & Qt::RightButton) {
        view3d_->mCamera.MovementStart(CAMERA_ZOOM, point);
    }
}

void OmView3dUi::CameraMovementMouseEnd(QMouseEvent* event)
{
    Vector2f point = Vector2f(event->x(), event->y());
    view3d_->mCamera.MovementEnd(point);
}

void OmView3dUi::CameraMovementMouseUpdate(QMouseEvent* event)
{
    Vector2f point = Vector2f(event->x(), event->y());
    view3d_->mCamera.MovementUpdate(point);
    view3d_->updateGL();
}

void OmView3dUi::CameraMovementMouseWheel(QWheelEvent* event)
{
    //assert(0);
    //printf("wheel\n");
    Vector2f point = Vector2f(event->x(), event->y());
    view3d_->mCamera.MovementStart(CAMERA_ZOOM, point);

    // fake mouse drag
    // TODO: make mouse zoom speed a preference
    const int numDegrees = event->delta() / 8;
    const int numSteps = numDegrees / 15;
    if(numSteps >= 0) {
        point.y -= 30.f;
    } else {
        point.y += 30.f;
    }
    view3d_->mCamera.MovementUpdate(point);
    view3d_->updateGL();
    view3d_->mCamera.MovementEnd(point);
}

/////////////////////////////////
///////          Segment Picking

SegmentDataWrapper OmView3dUi::PickSegmentMouse(QMouseEvent* event, const bool drag)
{
    //extract event properties
    Vector2i point2di(event->x(), event->y());

    const SegmentDataWrapper sdw = view3d_->PickPoint(point2di);
    if(!sdw.IsSegmentValid()){
        return SegmentDataWrapper();
    }

    //check if dragging
    if(drag && sdw == prevSDW_){
        return SegmentDataWrapper();
    } else {
        prevSDW_ = sdw;
    }

    return sdw;
}

/////////////////////////////////
///////           Segment Actions

void OmView3dUi::SegmentSelectToggleMouse(QMouseEvent* event, bool drag)
{
    //augment if shift pressed
    bool augment_selection = event->modifiers() & Qt::ShiftModifier;

    //get ids
    SegmentDataWrapper sdw = PickSegmentMouse(event, drag);
    if(!sdw.IsSegmentValid()){
        return;
    }

    //get segment state
    OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this, "view3dUi");
    if(augment_selection){
        sel.augmentSelectedSet_toggle(sdw.getID());
    } else {
        sel.selectJustThisSegment_toggle(sdw.getID());
    }

    sel.sendEvent();
}

/////////////////////////////////
///////           Segment Context Menu

void OmView3dUi::ShowSegmentContextMenu(QMouseEvent* event)
{
    const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);

    view3d_->updateGL();

    if(!pickPoint.sdw.IsSegmentValid()){
        return;
    }

    //refersh context menu and display
    segmentContextMenu_.Refresh(pickPoint, vgs_);
    segmentContextMenu_.exec(event->globalPos());
}

void OmView3dUi::centerAxisOfRotation(QMouseEvent* event)
{
    const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);

    view3d_->updateGL();

    if(!pickPoint.sdw.IsSegmentValid()){
        return;
    }

    view3d_->mCamera.SetFocus(pickPoint.voxel);
    view3d_->updateGL();
}

void OmView3dUi::crosshair(QMouseEvent* event)
{
    const OmSegmentPickPoint pickPoint = pickVoxelMouseCrosshair(event);

    view3d_->updateGL();

    if(!pickPoint.sdw.IsSegmentValid()){
        return;
    }

    vgs_->View2dState()->SetScaledSliceDepth(pickPoint.voxel);

    OmEvents::ViewCenterChanged();
}

OmSegmentPickPoint OmView3dUi::pickVoxelMouseCrosshair(QMouseEvent* event)
{
    //extract event properties
    const Vector2i point2di(event->x(), event->y());

    view3d_->updateGL();

    const SegmentDataWrapper sdw = view3d_->PickPoint(point2di);
    if(!sdw.IsSegmentValid()){
        return OmSegmentPickPoint();
    }

    Vector3f point3d;
    if(!view3d_->UnprojectPoint(point2di, point3d)){
        return OmSegmentPickPoint();
    }

    OmSegmentPickPoint ret = { sdw, point3d };

    return ret;
}

void OmView3dUi::resetWindow()
{
    static const Vector3f coord(0,0,0);

    view3d_->mCamera.SetFocus(coord);
    view3d_->mCamera.SetDistance(300.0);

    view3d_->updateGL();
}

bool OmView3dUi::gestureEvent(QGestureEvent *event) {
    return macGestures_->GestureEvent(event);
}
