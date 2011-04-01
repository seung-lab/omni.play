#include "actions/omActions.h"
#include "common/omDebug.h"
#include "events/omEvents.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
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
#include "view3d/omView3d.h"
#include "view3d/omView3dUi.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"

OmView3dUi::OmView3dUi(OmView3d* view3d, OmViewGroupState* vgs)
    : mpView3d(view3d)
    , vgs_(vgs)
    , macGestures_(new OmMacOSXGestures(view3d))
    , mCPressed(false)
{}

OmView3dUi::~OmView3dUi()
{}

void OmView3dUi::MousePressed(QMouseEvent* event){
    NavigationModeMousePressed(event);
}

void OmView3dUi::MouseRelease(QMouseEvent* event)
{
    if( om::tool::SPLIT == OmStateManager::GetToolMode()){
        DendModeMouseReleased(event);
        return;
    } else if(om::tool::CUT == OmStateManager::GetToolMode()) {
        CutModeMouseReleased(event);
        return;
    }

    NavigationModeMouseRelease(event);
}

void OmView3dUi::MouseMove(QMouseEvent* event)
{
    //ignore movement without buttons
    if (!event->buttons())
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
    {
        SegmentationDataWrapper sdw(1);
        OmSegmentCenter::CenterSegment(vgs_, sdw);
        break;
    }
    case Qt::Key_Escape:
        resetWindow();
        break;
    case Qt::Key_Minus:
        mpView3d->DoZoom(-1);
        break;
    case Qt::Key_Equal:
        mpView3d->DoZoom(1);
        break;
    }
}

void OmView3dUi::DendModeMouseReleased(QMouseEvent* event)
{
    DataCoord coord;
    SegmentDataWrapper sdw = PickVoxelMouseCrosshair(event, coord);
    if (!sdw.IsSegmentValid()) {
        mpView3d->updateGL();
        return;
    }
    mpView3d->updateGL();
    OmActions::FindAndSplitSegments(sdw, vgs_, coord);
}

void OmView3dUi::CutModeMouseReleased(QMouseEvent* event)
{
    const SegmentDataWrapper sdw = PickSegmentMouse(event, false);
    if (!sdw.IsSegmentValid()) {
        mpView3d->updateGL();
        return;
    }
    mpView3d->updateGL();
    OmActions::FindAndCutSegments(sdw, vgs_);
}

/////////////////////////////////
///////          Navigation Mode Methods

void OmView3dUi::NavigationModeMousePressed(QMouseEvent* event)
{
    if (event->buttons() & Qt::RightButton && !event->modifiers()) {
        ShowSegmentContextMenu(event);
        return;
    }

    bool control_modifier = event->modifiers() & Qt::ControlModifier;

    if (event->buttons() & Qt::LeftButton && control_modifier) {
        crosshair(event);
    } else if ((event->buttons() & Qt::LeftButton) && mCPressed){
        CenterAxisOfRotation(event);
    } else {
        CameraMovementMouseStart(event);
    }
}

void OmView3dUi::NavigationModeMouseRelease(QMouseEvent* event)
{
    CameraMovementMouseEnd(event);
}

void OmView3dUi::NavigationModeMouseMove(QMouseEvent* event)
{
    CameraMovementMouseUpdate(event);
}

void OmView3dUi::NavigationModeMouseDoubleClick(QMouseEvent* event)
{
    CenterAxisOfRotation(event);
}

void OmView3dUi::NavigationModeMouseWheel(QWheelEvent* event)
{
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
    if (event->buttons() & Qt::LeftButton && !shift_modifier) {
        mpView3d->mCamera.MovementStart(CAMERA_ORBIT_ROTATE, point);

        //left + shift moves lookat
    } else if (event->buttons() & Qt::LeftButton && shift_modifier) {
        mpView3d->mCamera.MovementStart(CAMERA_LOOKAT_ROTATE, point);

        //mid button pans
    } else if (event->buttons() & Qt::MidButton) {
        mpView3d->mCamera.MovementStart(CAMERA_PAN, point);

        //right button zooms
    } else if (event->buttons() & Qt::RightButton) {
        mpView3d->mCamera.MovementStart(CAMERA_ZOOM, point);
    }
}

void OmView3dUi::CameraMovementMouseEnd(QMouseEvent* event)
{
    Vector2f point = Vector2f(event->x(), event->y());
    mpView3d->mCamera.MovementEnd(point);
}

void OmView3dUi::CameraMovementMouseUpdate(QMouseEvent* event)
{
    Vector2f point = Vector2f(event->x(), event->y());
    mpView3d->mCamera.MovementUpdate(point);
    mpView3d->updateGL();
}

void OmView3dUi::CameraMovementMouseWheel(QWheelEvent* event)
{
    //assert(0);
    //printf("wheel\n");
    Vector2f point = Vector2f(event->x(), event->y());
    mpView3d->mCamera.MovementStart(CAMERA_ZOOM, point);

    // fake mouse drag
    // TODO: make mouse zoom speed a preference
    const int numDegrees = event->delta() / 8;
    const int numSteps = numDegrees / 15;
    if (numSteps >= 0) {
        point.y -= 30.f;
    } else {
        point.y += 30.f;
    }
    mpView3d->mCamera.MovementUpdate(point);
    mpView3d->updateGL();
    mpView3d->mCamera.MovementEnd(point);
}

/////////////////////////////////
///////          Segment Picking

SegmentDataWrapper OmView3dUi::PickSegmentMouse(QMouseEvent* event,
                                                bool drag,
                                                int *type)
{
    debug(3d, "OmView3dUi::PickSegmentMouse\n");

    //extract event properties
    Vector2i point2di(event->x(), event->y());

    //pick point causes localized redraw (but all depth info stored in selection buffer)
    int pickName;
    const SegmentDataWrapper sdw = mpView3d->PickPoint(point2di, pickName);
    if(!sdw.IsSegmentValid()){
        return SegmentDataWrapper();
    }

    //check if dragging
    if (drag && sdw == prevSDW_){
        return SegmentDataWrapper();
    } else {
        prevSDW_ = sdw;
    }

    //if valid pointer, then store pick name
    if (type != NULL) {
        *type = pickName;
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
    int pick_object_type;
    SegmentDataWrapper sdw = PickSegmentMouse(event, drag, &pick_object_type);
    if (!sdw.IsSegmentValid()){
        return;
    }

    //if picked type was not a mesh
    if (pick_object_type != OMGL_NAME_MESH){
        return;
    }

    //get segment state
    OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this, "view3dUi" );
    if( augment_selection ){
        sel.augmentSelectedSet_toggle( sdw.getID() );
    } else {
        sel.selectJustThisSegment_toggle( sdw.getID() );
    }
    sel.sendEvent();
}

/////////////////////////////////
///////           Segment Context Menu

void OmView3dUi::ShowSegmentContextMenu(QMouseEvent* event)
{
    DataCoord coord;
    SegmentDataWrapper sdw = PickVoxelMouseCrosshair(event, coord);
    if (!sdw.IsSegmentValid()){
        mpView3d->updateGL();
        return;
    }
    mpView3d->updateGL();

    //refersh context menu and display
    mSegmentContextMenu.Refresh( sdw, vgs_, coord);
    mSegmentContextMenu.exec(event->globalPos());
}

void OmView3dUi::CenterAxisOfRotation(QMouseEvent* event)
{
    DataCoord voxel;
    SegmentDataWrapper sdw = PickVoxelMouseCrosshair(event, voxel);
    mpView3d->updateGL();

    if(!sdw.IsSegmentValid()){
        return;
    }

    mpView3d->mCamera.SetFocus(voxel);
    mpView3d->updateGL();

    mCPressed = false;
}

void OmView3dUi::crosshair(QMouseEvent* event)
{
    DataCoord voxel;
    const SegmentDataWrapper sdw = PickVoxelMouseCrosshair(event, voxel);
    mpView3d->updateGL();

    if(!sdw.IsSegmentValid()){
        return;
    }

    vgs_->View2dState()->SetScaledSliceDepth(voxel);

    OmEvents::ViewCenterChanged();
}

SegmentDataWrapper OmView3dUi::PickVoxelMouseCrosshair(QMouseEvent* event,
                                                       DataCoord& rVoxel)
{
    //extract event properties
    const Vector2i point2di(event->x(), event->y());

    //pick point causes localized redraw (but all depth info stored in selection buffer)
    int pickName;
    mpView3d->updateGL();

    const SegmentDataWrapper sdw = mpView3d->PickPoint(point2di, pickName);
    if(!sdw.IsSegmentValid()){
        return SegmentDataWrapper();
    }

    //unproject to point3d
    Vector3f point3d;
    if (!mpView3d->UnprojectPoint(point2di, point3d)){
        return SegmentDataWrapper();
    }

    //define depth scale factor
    const float z_depth_scale = 0.0f;

    //normalized vector from camera to unprojected point
    Vector3f cam_to_point = (point3d - mpView3d->mCamera.GetPosition());
    cam_to_point.normalize();
    Vector3f scaled_norm_vec = cam_to_point* z_depth_scale;

    //get voxel at point3d
    OmSegmentation & current_seg = sdw.GetSegmentation();
    NormCoord norm_coord = current_seg.Coords().DataToNormCoord(point3d + scaled_norm_vec);
    DataCoord voxel = current_seg.Coords().NormToDataCoord(norm_coord);

    //return success with voxel
    rVoxel = voxel;
    return sdw;
}

void OmView3dUi::resetWindow()
{
    static const Vector3f coord(0,0,0);

    mpView3d->mCamera.SetFocus(coord);
    mpView3d->mCamera.SetDistance(300.0);

    mpView3d->updateGL();
}

bool OmView3dUi::gestureEvent(QGestureEvent *event) {
    return macGestures_->GestureEvent(event);
}
