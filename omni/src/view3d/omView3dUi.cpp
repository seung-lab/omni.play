
#include "omView3dUi.h"
#include "omView3d.h"
#include "omCamera.h"

#include "project/omProject.h"
#include "segment/omSegmentEditor.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/voxel/omVoxelSelectionAction.h"
#include "segment/actions/voxel/omVoxelSetValueAction.h"

#include "volume/omVolume.h"
#include "volume/omDrawOptions.h"

#include "system/omStateManager.h"
#include "common/omDebug.h"

#define DEBUG 0

/////////////////////////////////
///////
///////          Example Class
///////

OmView3dUi::OmView3dUi(OmView3d * view3d, OmViewGroupState * vgs )
	: mpView3d(view3d), mViewGroupState(vgs)
{
        mCPressed = false;
}

/////////////////////////////////
///////          Example Methods

void
 OmView3dUi::MousePressed(QMouseEvent * event)
{
	switch (OmStateManager::GetSystemMode()) {
	case DEND_MODE:
	case NAVIGATION_SYSTEM_MODE:
		NavigationModeMousePressed(event);
		break;

	case EDIT_SYSTEM_MODE:
		EditModeMousePressed(event);
		break;
	}
}

void OmView3dUi::MouseRelease(QMouseEvent * event)
{
	switch (OmStateManager::GetSystemMode()) {
	case DEND_MODE:
		DendModeMouseReleased(event);
		break;
	case NAVIGATION_SYSTEM_MODE:
		NavigationModeMouseRelease(event);
		break;

	case EDIT_SYSTEM_MODE:
		EditModeMouseRelease(event);
		break;
	}
}

void OmView3dUi::MouseMove(QMouseEvent * event)
{
	//ignore movement without buttons
	if (!event->buttons())
		return;

	switch (OmStateManager::GetSystemMode()) {
	case DEND_MODE:
	case NAVIGATION_SYSTEM_MODE:
		NavigationModeMouseMove(event);
		break;

	case EDIT_SYSTEM_MODE:
		EditModeMouseMove(event);
		break;
	}
}

void OmView3dUi::MouseDoubleClick(QMouseEvent * event)
{
	switch (OmStateManager::GetSystemMode()) {
	case DEND_MODE:
	case NAVIGATION_SYSTEM_MODE:
		NavigationModeMouseDoubleClick(event);
		break;

	case EDIT_SYSTEM_MODE:
		EditModeMouseDoubleClick(event);
		break;
	}
}

void OmView3dUi::MouseWheel(QWheelEvent * event)
{
	switch (OmStateManager::GetSystemMode()) {
	case DEND_MODE:
	case NAVIGATION_SYSTEM_MODE:
		NavigationModeMouseWheel(event);
		break;

	case EDIT_SYSTEM_MODE:
		EditModeMouseWheel(event);
		break;
	}
}

void OmView3dUi::KeyPress(QKeyEvent * event)
{
        if (event->key() == Qt::Key_C) mCPressed = true; 
	switch (OmStateManager::GetSystemMode()) {
	case DEND_MODE:
	case NAVIGATION_SYSTEM_MODE:
		NavigationModeKeyPress(event);
		break;

	case EDIT_SYSTEM_MODE:
		EditModeKeyPress(event);
		break;
	}
}

void OmView3dUi::DendModeMouseReleased(QMouseEvent * event)
{
	debug("dend3d", "OmView3dUi::DendModeMouseReleased\n");
        //get segment
        OmId segmentation_id, segment_id;
        if (!PickSegmentMouse(event, false, segmentation_id, segment_id)) {
                mpView3d->updateGL();
                return;
        }
        mpView3d->updateGL();


	OmSegmentation & r_segmentation = OmProject::GetSegmentation(segmentation_id);
	OmSegment * seg = r_segmentation.GetSegment(segment_id);

        seg->splitTwoChildren(seg);
        OmStateManager::SetSystemModePrev();
}

/////////////////////////////////
///////          Navigation Mode Methods

void OmView3dUi::NavigationModeMousePressed(QMouseEvent * event)
{
	if (event->buttons() & Qt::RightButton && !event->modifiers()) {
		ShowSegmentContextMenu(event);
		return;
	}

	bool control_modifier = event->modifiers() & Qt::ControlModifier;
	debug("hey","%i  %i \n\n",control_modifier,mCPressed);
	if (event->buttons() & Qt::LeftButton && control_modifier) {
		crosshair(event);
	} else if ((event->buttons() & Qt::LeftButton) && mCPressed){
	        CenterAxisOfRotation(event);
	} else {
		CameraMovementMouseStart(event);
	}
}

void OmView3dUi::NavigationModeMouseRelease(QMouseEvent * event)
{
	CameraMovementMouseEnd(event);
}

void OmView3dUi::NavigationModeMouseMove(QMouseEvent * event)
{
	CameraMovementMouseUpdate(event);
}

void OmView3dUi::NavigationModeMouseDoubleClick(QMouseEvent *)
{
	//SegmentSelectToggleMouse(event, false);
}

void OmView3dUi::NavigationModeMouseWheel(QWheelEvent* event)
{
	CameraMovementMouseWheel(event);
}

void OmView3dUi::NavigationModeKeyPress(QKeyEvent *)
{
}

/////////////////////////////////
///////          Edit Mode Methods

void OmView3dUi::EditModeMousePressed(QMouseEvent * event)
{

	//if right click and no modifiers
	if (event->buttons() & Qt::RightButton && !event->modifiers()) {
		ShowSegmentContextMenu(event);
		return;
	}
	//otherwise check modifiers
	switch (event->modifiers() & (Qt::ControlModifier | Qt::MetaModifier)) {
	case Qt::ControlModifier:
		VoxelEditMouse(event, false);
		return;

	case Qt::MetaModifier:
		ShowSegmentContextMenu(event);
		return;

	default:
		CameraMovementMouseStart(event);
		return;
	}

}

void OmView3dUi::EditModeMouseRelease(QMouseEvent * event)
{

	CameraMovementMouseEnd(event);
#if 0
	switch (event->modifiers() & (Qt::ControlModifier)) {
	case Qt::ControlModifier:
		CameraMovementMouseEnd(event);
		return;
	}
#endif
}

void OmView3dUi::EditModeMouseMove(QMouseEvent * event)
{

	switch (event->modifiers() & (Qt::ControlModifier)) {
	case Qt::ControlModifier:
		VoxelEditMouse(event, true);
		return;

	default:
		CameraMovementMouseUpdate(event);
		return;
	}
}

void OmView3dUi::EditModeMouseDoubleClick(QMouseEvent * event)
{
	SegmentSelectToggleMouse(event, false);
}

void OmView3dUi::EditModeMouseWheel(QWheelEvent * event)
{
	CameraMovementMouseWheel(event);
}


void OmView3dUi::EditModeKeyPress(QKeyEvent * event)
{
	NavigationModeKeyPress(event);
}

/////////////////////////////////
///////          Camera Movement Methods

void OmView3dUi::CameraMovementMouseStart(QMouseEvent * event)
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

void OmView3dUi::CameraMovementMouseEnd(QMouseEvent * event)
{
	Vector2f point = Vector2f(event->x(), event->y());
	mpView3d->mCamera.MovementEnd(point);
}

void OmView3dUi::CameraMovementMouseUpdate(QMouseEvent * event)
{
	Vector2f point = Vector2f(event->x(), event->y());
	mpView3d->mCamera.MovementUpdate(point);
	mpView3d->updateGL();
}

void OmView3dUi::CameraMovementMouseWheel(QWheelEvent * event)
{
	//debug("view3d", "in %s...\n", __FUNCTION__ );
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

bool OmView3dUi::PickSegmentMouse(QMouseEvent * event, bool drag, OmId & segmentationId, OmId & segmentId, int *type)
{
	debug("3d", "OmView3dUi::PickSegmentMouse\n");

	//extract event properties
	Vector2i point2d(event->x(), event->y());

	//pick point causes localized redraw (but all depth info stored in selection buffer)
	vector<unsigned int> result;
	bool valid_pick = mpView3d->PickPoint(point2d, result);

	//if valid and return count
	if (!valid_pick || (result.size() != 3))
		return false;

	//ensure valid OmIds
	if (!OmProject::IsSegmentationValid(result[0]))
		return false;
	if (!OmProject::GetSegmentation(result[0]).IsSegmentValid(result[1]))
		return false;

	//check if dragging
	if (drag && 
	    (result[0] == mPrevSegmentationId) && 
	    (result[1] == mPrevSegmentId)) {
		return false;
	} else {
		//update prev selection
		mPrevSegmentationId = result[0];
		mPrevSegmentId = result[1];
	}

	//otherwise vaild segment
	segmentationId = result[0];
	segmentId = result[1];
	//debug("genone","OmView3dUi::PickSegmentMouse %i \n",result[1]);       

	//if valid pointer, then store pick name
	if (type != NULL) {
		*type = result[2];
	}

	return true;
}

/////////////////////////////////
///////          Pick Voxel Methods


bool OmView3dUi::PickVoxelMouse(QMouseEvent * event, bool drag, DataCoord & rVoxel)
{

	//extract event properties
	Vector2i point2d(event->x(), event->y());

	//pick point causes localized redraw (but all depth info stored in selection buffer)
	vector<unsigned int>result;
	bool valid_pick = mpView3d->PickPoint(point2d, result);

	//if valid and return count
	if (!valid_pick || (result.size() != 3))
		return false;

	//last name must be voxel
	if (OMGL_NAME_VOXEL != result[2])
		return false;

	//unproject to point3d
	Vector3f point3d;
	if (!mpView3d->UnprojectPoint(point2d, point3d))
		return false;

	//define depth scale factor
	float z_depth_scale = 1.0f;

	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		//scales closer (on top of voxel)
		z_depth_scale = -0.01f;
		break;

	case SUBTRACT_VOXEL_MODE:
		//scales further away (into voxel)
		z_depth_scale = 0.01f;
		break;

	default:
		return false;
	}

	//normalized vector from camera to unprojected point
	Vector3f cam_to_point = (point3d - mpView3d->mCamera.GetPosition());
	cam_to_point.normalize();
	Vector3f scaled_norm_vec = cam_to_point * z_depth_scale;

	//TODO: if left button, ensure voxel exists

	//get voxel at point3d
        OmSegmentation & current_seg = OmProject::GetSegmentation(result[0]);
	NormCoord norm_coord = current_seg.SpaceToNormCoord(point3d + scaled_norm_vec);
	DataCoord voxel = current_seg.NormToDataCoord(norm_coord);

	//if we've already selected voxel while dragging
	if (drag && (mPrevMouseSelectVoxel == voxel)) {
		return false;
	} else {
		mPrevMouseSelectVoxel = voxel;
	}

	//return success with voxel
	rVoxel = voxel;
	return true;
}

/////////////////////////////////
///////           Segment Actions

void OmView3dUi::SegmentSelectToggleMouse(QMouseEvent * event, bool drag)
{
	//augment if shift pressed
	bool augment_selection = event->modifiers() & Qt::ShiftModifier;

	//get ids
	OmId segmentation_id, segment_id;
	int pick_object_type;
	if (!PickSegmentMouse(event, drag, segmentation_id, segment_id, &pick_object_type))
		return;

	//if picked type was not a mesh
	if (pick_object_type != OMGL_NAME_MESH)
		return;

	//get segment state
	bool new_segment_select_state = !(OmProject::GetSegmentation(segmentation_id).IsSegmentSelected(segment_id));

	//if not augmenting slection and selecting segment
	if (!augment_selection && new_segment_select_state) {
		//get current selection
		OmSegmentation & r_segmentation = OmProject::GetSegmentation(segmentation_id);
		//select new segment, deselect current segments
		OmIds select_segment_ids;
		select_segment_ids.insert(segment_id);
		(new
		 OmSegmentSelectAction(segmentation_id, select_segment_ids,
				       r_segmentation.GetSelectedSegmentIds()))->Run();

	} else {
		//set state of 
		(new OmSegmentSelectAction(segmentation_id, segment_id, new_segment_select_state))->Run();
	}
}

/////////////////////////////////
///////           Voxel Actions

void OmView3dUi::VoxelEditMouse(QMouseEvent * mouseEvent, bool drag)
{

	switch (OmStateManager::GetToolMode()) {
	case SELECT_VOXEL_MODE:
		VoxelSelectToggleMouse(mouseEvent, drag);
		break;

	case ADD_VOXEL_MODE:
	case SUBTRACT_VOXEL_MODE:
		VoxelSetMouse(mouseEvent, drag);
		break;

	case SELECT_MODE:
	case PAN_MODE:
	case CROSSHAIR_MODE:
	case ZOOM_MODE:
	case FILL_MODE:
	case VOXELIZE_MODE:
		break;
	}

}

void OmView3dUi::VoxelSelectToggleMouse(QMouseEvent * mouseEvent, bool drag)
{

	//augment if shift pressed
	bool augment_selection = mouseEvent->modifiers() & Qt::ShiftModifier;

	//pick voxel
	DataCoord picked_voxel;
	if (!PickVoxelMouse(mouseEvent, drag, picked_voxel))
		return;

	//get voxel state
	bool voxel_select_state;
	if (drag) {
		voxel_select_state = mPrevSelectionState;
	} else {
		voxel_select_state = OmSegmentEditor::GetSelectedVoxelState(picked_voxel);
		mPrevSelectionState = voxel_select_state;
	}

	//run action
	(new OmVoxelSelectionAction(picked_voxel, !voxel_select_state, augment_selection))->Run();
}

void OmView3dUi::VoxelSetMouse(QMouseEvent * mouseEvent, bool drag)
{

	//pick voxel
	DataCoord picked_voxel;
	if (!PickVoxelMouse(mouseEvent, drag, picked_voxel))
		return;

	//get voxel state
	bool voxel_select_state;
	if (drag) {
		voxel_select_state = mPrevSelectionState;
	} else {
		voxel_select_state = OmSegmentEditor::GetSelectedVoxelState(picked_voxel);
		mPrevSelectionState = voxel_select_state;
	}

	//run action
	//(new OmVoxelSetAction(picked_voxel))->Run();

	//store current selection
	OmId segmentation_id, segment_id;
	bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);

	//return if not valid
	if (!valid_edit_selection)
		return;

	//switch on tool mode
	SEGMENT_DATA_TYPE data_value;
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		//get value associated to segment id
		data_value = OmProject::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
		break;

	case SUBTRACT_VOXEL_MODE:
		data_value = NULL_SEGMENT_DATA;
		break;

	default:
		assert(false);
	}

	//run action
	(new OmVoxelSetValueAction(segmentation_id, picked_voxel, data_value))->Run();
}

/////////////////////////////////
///////           Segment Context Menu

void OmView3dUi::ShowSegmentContextMenu(QMouseEvent * event)
{

	//get segment
	OmId segmentation_id, segment_id;
	if (!PickSegmentMouse(event, false, segmentation_id, segment_id)) {
		mpView3d->updateGL();
		return;
	}
	mpView3d->updateGL();

	//refersh context menu and display
	mSegmentContextMenu.Refresh(segmentation_id, segment_id);
	mSegmentContextMenu.exec(event->globalPos());
}

void OmView3dUi::CenterAxisOfRotation(QMouseEvent * event)
{
	DataCoord voxel;
	OmId seg = PickVoxelMouseCrosshair(event, voxel);
	if (!seg) {
		mpView3d->updateGL();
		return;
	}
	mpView3d->updateGL();

        OmSegmentation & current_seg = OmProject::GetSegmentation(seg);
	SpaceCoord picked_voxel = current_seg.NormToSpaceCoord(current_seg.DataToNormCoord(voxel));
	mpView3d->mCamera.SetFocus(picked_voxel);
	mpView3d->updateGL();

	mCPressed = false;
}

void OmView3dUi::crosshair(QMouseEvent * event)
{
	debug("view3d", "hi from %s\n", __FUNCTION__);

	DataCoord voxel;
	OmId seg = PickVoxelMouseCrosshair(event, voxel);
	if (!seg) {
		mpView3d->updateGL();
		return;
	}
	mpView3d->updateGL();

	debug("view3d", "coordinate is (%d, %d, %d)\n", voxel.x, voxel.y, voxel.z );

        OmSegmentation & current_seg = OmProject::GetSegmentation(seg);
	SpaceCoord picked_voxel = current_seg.NormToSpaceCoord(current_seg.DataToNormCoord(voxel));

	mViewGroupState->SetViewSliceDepth(YZ_VIEW, picked_voxel.x );
	mViewGroupState->SetViewSliceDepth(XY_VIEW, picked_voxel.z );
	mViewGroupState->SetViewSliceDepth(XZ_VIEW, picked_voxel.y );
	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
	
	debug("view3d", "coordinate is now (%f, %f, %f)\n", 
	      picked_voxel.x,
	      picked_voxel.y,
	      picked_voxel.z
	      );
}

OmId OmView3dUi::PickVoxelMouseCrosshair(QMouseEvent * event, DataCoord & rVoxel)
{
        //extract event properties
        Vector2i point2d(event->x(), event->y());

        //pick point causes localized redraw (but all depth info stored in selection buffer)
        vector<unsigned int>result;

	bool valid_pick;
	mpView3d->updateGL();
	valid_pick = mpView3d->PickPoint(point2d, result);

	debug("crosshair", "valid_pick = %i, size of crosshair PickPoint call's hit list: %i\n", 
	      valid_pick, result.size());

	if(!valid_pick || result.size() != 3)
		return 0;

        if (!OmProject::IsSegmentationValid(result[0]))
                return 0;
        if (!OmProject::GetSegmentation(result[0]).IsSegmentValid(result[1]))
                return 0;

        //unproject to point3d
        Vector3f point3d;
        if (!mpView3d->UnprojectPoint(point2d, point3d))
                return 0;

        //define depth scale factor
        float z_depth_scale = 0.0f;

        //normalized vector from camera to unprojected point
        Vector3f cam_to_point = (point3d - mpView3d->mCamera.GetPosition());
        cam_to_point.normalize();
        Vector3f scaled_norm_vec = cam_to_point * z_depth_scale;

        //get voxel at point3d
	OmSegmentation & current_seg = OmProject::GetSegmentation(result[0]);
        NormCoord norm_coord = current_seg.SpaceToNormCoord(point3d + scaled_norm_vec);
        DataCoord voxel = current_seg.NormToDataCoord(norm_coord);

        //return success with voxel
        rVoxel = voxel;
        return result[0];
}
