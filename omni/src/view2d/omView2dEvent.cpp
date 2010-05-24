#include "project/omProject.h"
#include "omView2d.h"
#include "gui/toolbars/dendToolbar.h"
#include "segment/actions/segment/omSegmentSelectionAction.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/voxel/omVoxelSetValueAction.h"
#include "segment/omSegmentEditor.h"
#include "segment/omSegment.h"
#include "system/omStateManager.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omLocalPreferences.h"
#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "volume/omVolume.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

#include "project/omProject.h"

/**
 * \name Mouse Event Handlers 
 */

//\{
void OmView2d::mouseDoubleClickEvent(QMouseEvent*)
{
	switch (OmStateManager::GetSystemMode()) {
	case NAVIGATION_SYSTEM_MODE:
	case DEND_MODE:
	case EDIT_SYSTEM_MODE:
		break;
	}
}

void OmView2d::mousePressEvent(QMouseEvent * event)
{
	clickPoint.x = event->x();
	clickPoint.y = event->y();
	rememberCoord = ScreenToSpaceCoord(clickPoint);
	rememberDepthCoord = mViewGroupState->GetViewDepthCoord();

	switch (OmStateManager::GetSystemMode()) {

	case NAVIGATION_SYSTEM_MODE: 
		if (event->button() == Qt::LeftButton) {
			const bool crosshair = event->modifiers() & Qt::ControlModifier;
			if( crosshair ){
				mouseSetCrosshair(event);
			} else {
				mouseNavModeLeftButton(event);
			}
		} else if (event->button() == Qt::RightButton) {
			mouseSelectSegment(event);
		}
			
		cameraMoving = true;
			
		break;
			
  	case DEND_MODE: 
		if (event->button() == Qt::LeftButton) {
			doFindAndSplitSegment( event );
			doRedraw();
		}
		break;
		
	case EDIT_SYSTEM_MODE: 
		if (event->button() == Qt::LeftButton) {
			const bool crosshair = event->modifiers() & Qt::ControlModifier;
			if( crosshair ){
				mouseSetCrosshair(event);
			} else {
				mouseEditModeLeftButton(event);
			}
		} else if (event->button() == Qt::RightButton) {
			mouseSelectSegment(event);
		}
		
		break;
	}
}

void OmView2d::doRedraw()
{
	Refresh();
	mTextures.clear();
	myUpdate();
}

void OmView2d::mouseSetCrosshair(QMouseEvent * event)
{
	doRedraw();

	SetDepth(event);

	OmStateManager::SetToolMode(PAN_MODE);
	OmEventManager::PostEvent(new OmSystemModeEvent(OmSystemModeEvent::SYSTEM_MODE_CHANGE));
}

DataCoord OmView2d::getMouseClickpointLocalDataCoord(QMouseEvent * event)
{
	Vector2f clickPoint = Vector2f(event->x(), event->y());
	return ScreenToDataCoord(clickPoint);
}

DataCoord OmView2d::getMouseClickpointGlobalDataCoord(QMouseEvent * event)
{
	return  getMouseClickpointLocalDataCoord(event);
}

void OmView2d::doSelectSegment( SegmentDataWrapper sdw, bool augment_selection )
{
	OmSegmentation & segmentation = sdw.getSegmentation();
	OmId segmentID = sdw.getID();

	if( !segmentation.IsSegmentValid(segmentID)){
		return;
	}

	OmSegmentEditor::SetEditSelection( segmentation.GetId(), segmentID);

	const bool curSegmentNotYetMarkedAsSelected = !(segmentation.IsSegmentSelected(segmentID));

	// if not augmenting slection and selecting segment, then 
	//  select new segment, and deselect current segment(s)
	if (!augment_selection && curSegmentNotYetMarkedAsSelected) {
		OmSegmentSelectAction ssa;
					    

		OmIds select_segment_ids;
		select_segment_ids.insert(segmentID);
		(new OmSegmentSelectAction( segmentation.GetId(),
					    select_segment_ids,
					    segmentation.GetSelectedSegmentIds(), 
					    segmentID))->Run();
	} else {
		(new OmSegmentSelectAction( segmentation.GetId(),
					    segmentID, 
					    curSegmentNotYetMarkedAsSelected, 
					    segmentID))->Run();
	}

	Refresh();
	mTextures.clear();
	myUpdate();
}

void OmView2d::mouseSelectSegment(QMouseEvent * event)
{
	bool augment_selection = event->modifiers() & Qt::ShiftModifier;

	SegmentDataWrapper * sdw = getSelectedSegment( event );
	if( NULL == sdw ){
		return;
	}

	return doSelectSegment( (*sdw), augment_selection );
}

SegmentDataWrapper * OmView2d::getSelectedSegment( QMouseEvent * event )
{
	DataCoord dataClickPoint = getMouseClickpointGlobalDataCoord(event);

	OmId segmentID;
	OmId segmentationID;
	if( SEGMENTATION == mVolumeType ){
		segmentationID = mImageId;
		OmSegmentation & segmentation = OmProject::GetSegmentation(segmentationID);
		segmentID = segmentation.GetVoxelSegmentId(dataClickPoint);
		if( 0 == segmentID  ){
			return NULL;
		}
		return new SegmentDataWrapper( segmentationID, segmentID );
	} else {
		OmChannel & current_channel = OmProject::GetChannel(mImageId);
		foreach( OmId id, current_channel.GetValidFilterIds() ) {

			OmFilter2d &filter = current_channel.GetFilter(id);

			if (filter.GetSegmentation() ) {
				segmentationID = filter.GetSegmentation();
				OmSegmentation & segmentation = OmProject::GetSegmentation(segmentationID);
				segmentID = segmentation.GetVoxelSegmentId(dataClickPoint);
				if( 0 == segmentID  ){
					return NULL;
				}
				return new SegmentDataWrapper( segmentationID, segmentID );
			}
		}
	}
	
	return NULL;
}

void OmView2d::EditMode_MouseRelease_LeftButton_Filling(QMouseEvent * event)
{

	mScribbling = false;
	DataCoord dataClickPoint = getMouseClickpointLocalDataCoord(event);
	DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);

	//store current selection
	OmId segmentation_id, segment_id;
	bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);

	//return if not valid
	if (!valid_edit_selection)
		return;

	//switch on tool mode
	OmSegID data_value;
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		//get value associated to segment id
		data_value = segment_id;
		break;

	case SUBTRACT_VOXEL_MODE:
		data_value = NULL_SEGMENT_DATA;
		break;

	default:
		return;
		break;
	}

	OmId segid = OmProject::GetSegmentation(segmentation_id).GetVoxelSegmentId(globalDataClickPoint);
	FillToolFill(segmentation_id, globalDataClickPoint, data_value, segid);

	doRedraw();
}

void OmView2d::EditModeMouseRelease(QMouseEvent * event)
{
	// END PAINTING

	bool doselection = false;

	if (event->button() == Qt::RightButton) {
		return;
	}

	if (event->button() == Qt::LeftButton) {
		if (amInFillMode()) {
			EditMode_MouseRelease_LeftButton_Filling(event);
		} else if (mScribbling) {

			mScribbling = false;
			DataCoord dataClickPoint = getMouseClickpointLocalDataCoord(event);
			DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);

			//store current selection
			OmId segmentation_id, segment_id;
			bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);

			//return if not valid
			if (!valid_edit_selection)
				return;

			//switch on tool mode
			OmSegID data_value;
			switch (OmStateManager::GetToolMode()) {
			case ADD_VOXEL_MODE:
				//get value associated to segment id
				data_value = segment_id;
				break;

			case SUBTRACT_VOXEL_MODE:
				data_value = NULL_SEGMENT_DATA;
				break;

			case SELECT_VOXEL_MODE:
				doselection = true;
				break;

			default:
				return;
				break;
			}

			//run action
			if (!doselection)
				BrushToolApplyPaint(segmentation_id, globalDataClickPoint, data_value);
			else
				PickToolAddToSelection(segmentation_id, globalDataClickPoint);

			lastDataPoint = dataClickPoint;

			myUpdate();
		}
	}
}

void OmView2d::EditModeMouseMove(QMouseEvent * event)
{
	// KEEP PAINTING
	//debug ("genone", "scribbling? %i!\n", mScribbling);

	
	if (PAN_MODE == OmStateManager::GetToolMode()) {
		mouseMove_NavMode_CamMoving(event);
	} else if (mScribbling) {
		EditMode_MouseMove_LeftButton_Scribbling(event);
		return;
	}
}
void OmView2d::wheelEvent(QWheelEvent * event)
{
	//debug("genone","OmView2d::wheelEvent -- " << mViewType);

	const int numDegrees = event->delta() / 8;
	const int numSteps = numDegrees / 15;

	const bool move_through_stack = event->modifiers() & Qt::ControlModifier;

	if (move_through_stack) {
		if (numSteps >= 0) {
			MoveUpStackCloserToViewer();
		} else {
			MoveDownStackFartherFromViewer();
		}
	} else {
		MouseWheelZoom(numSteps);
	}

	event->accept();

	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}

void OmView2d::MouseWheelZoom(const int numSteps)
{
	if (numSteps >= 0) {
		// ZOOMING IN

		Vector2 < int >current_zoom = mViewGroupState->GetZoomLevel();

		if (!mLevelLock && (current_zoom.y >= 10) && (current_zoom.x > 0)) {
			// need to move to previous mip level
			Vector2 <int> new_zoom = Vector2 < int >(current_zoom.x - 1, 6);
		        PanAndZoom(new_zoom);
		} else{
			Vector2 <int> new_zoom=Vector2i(current_zoom.x,current_zoom.y+ (1 * numSteps));
		        PanAndZoom(new_zoom);
		}
	} else {
		// ZOOMING OUT

		Vector2 < int >current_zoom = mViewGroupState->GetZoomLevel();

		if (!mLevelLock && (current_zoom.y <= 6) && (current_zoom.x < mRootLevel)) {
			// need to move to next mip level
			Vector2i new_zoom = Vector2i(current_zoom.x + 1, 10);
			PanAndZoom(new_zoom);	
		}

		else if (current_zoom.y > 1) {
			int zoom = current_zoom.y - (1 * (-1 * numSteps));
			if (zoom < 1)
				zoom = 1;

			Vector2i new_zoom = Vector2 < int >(current_zoom.x, zoom);
		        PanAndZoom(new_zoom);
		}
	}
}

void OmView2d::mouseZoomIn()
{
	MouseWheelZoom(15);
}

void OmView2d::mouseZoomOut()
{
	MouseWheelZoom(-15);
}

void OmView2d::mouseZoom(QMouseEvent * event)
{
	const bool zoomOut = event->modifiers() & Qt::ControlModifier;

	if (zoomOut) {
		mouseZoomOut();
	} else {
		mouseZoomIn();
	}
}

void OmView2d::mouseNavModeLeftButton(QMouseEvent * event)
{
	switch (OmStateManager::GetToolMode()) {
	case SELECT_MODE:
		mouseSelectSegment(event);
		break;
	case PAN_MODE:
		// dealt with in mouseMoveEvent()
		break;
	case CROSSHAIR_MODE:
		mouseSetCrosshair(event);
		break;
	case ZOOM_MODE:
		mouseZoom(event);
		OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
		break;
	case ADD_VOXEL_MODE:
		break;
	case SUBTRACT_VOXEL_MODE:
		break;
	case SELECT_VOXEL_MODE:
		break;
	case VOXELIZE_MODE:
		break;
	default:
		break;
	}
}

void OmView2d::mouseEditModeLeftButton(QMouseEvent * event)
{
	debug ("view2d", "OmView2d::mouseEditModeLeftButton %i,%i\n", SELECT_MODE, OmStateManager::GetToolMode());
	bool doselection = false;
	bool dosubtract = false;
	mScribbling = true;
	
	OmSegID data_value;

	switch (OmStateManager::GetToolMode()) {
	case SELECT_MODE:
		// debug ("genone", "Here!\n");
		mouseSelectSegment(event);
		return;
		break;
	case PAN_MODE:
		clickPoint.x = event->x();
		clickPoint.y = event->y();

		// dealt with in mouseMoveEvent()
		return;
		break;
	case CROSSHAIR_MODE:
		mouseSetCrosshair(event);
		return;
		break;
	case ZOOM_MODE:
		mouseZoom(event);
		OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
		return;
		break;
	case ADD_VOXEL_MODE:
		break;
	case SUBTRACT_VOXEL_MODE:
		dosubtract = true;
		break;
	case SELECT_VOXEL_MODE:	
		return;
		break;
	case VOXELIZE_MODE:
		return;
		break;
	default:
		break;
	}

	DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);
	OmId segmentation_id, segment_id;
	if (OmSegmentEditor::GetEditSelection(segmentation_id, segment_id)) {
		//run action
		if (!doselection) {
			if (dosubtract) {
				data_value = NULL_SEGMENT_DATA;
			} else {
				data_value = segment_id;
			}
			BrushToolApplyPaint(segmentation_id, globalDataClickPoint, data_value);
		} else {
			PickToolAddToSelection(segmentation_id, globalDataClickPoint);
		}
	} else {
		debug("genone", "No segment_id in edit selection\n");
	}

	lastDataPoint = getMouseClickpointLocalDataCoord(event);;

	myUpdate();

}

void OmView2d::mouseMoveEvent(QMouseEvent * event)
{

	mMousePoint = Vector2f(event->x(), event->y());

	// http://qt.nokia.com/doc/4.5/qt.html#MouseButton-enum
	if (event->buttons() != Qt::LeftButton) {
		// do nothing
	} else {

		switch (OmStateManager::GetSystemMode()) {
		case NAVIGATION_SYSTEM_MODE:
			if (cameraMoving) {
				if (PAN_MODE == OmStateManager::GetToolMode()) {
					mouseMove_NavMode_CamMoving(event);
					OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
				}
			}
			break;
		case DEND_MODE:
			break;
		case EDIT_SYSTEM_MODE:
			EditModeMouseMove(event);
			break;
		}
	}

	myUpdate();
}

void OmView2d::EditMode_MouseMove_LeftButton_Scribbling(QMouseEvent * event)
{
	bool doselection = false;

	DataCoord dataClickPoint = getMouseClickpointLocalDataCoord(event);
	DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);

	//store current selection
	OmId segmentation_id, segment_id;
	bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);

	//return if not valid
	if (!valid_edit_selection)
		return;

	//switch on tool mode
	OmSegID data_value;
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		//get value associated to segment id
		data_value = segment_id;
		break;

	case SUBTRACT_VOXEL_MODE:
		data_value = NULL_SEGMENT_DATA;
		break;

	case SELECT_VOXEL_MODE:
		doselection = true;
		break;

	default:
		return;
		break;
	}

	if (!doselection) {
		//run action
		BrushToolApplyPaint(segmentation_id, globalDataClickPoint, data_value);
		bresenhamLineDraw(lastDataPoint, dataClickPoint);
	} else {
		// TODO: bug here; ask MattW
		PickToolAddToSelection(segmentation_id, globalDataClickPoint);
	}

	lastDataPoint = dataClickPoint;

	myUpdate();
}

void OmView2d::mouseReleaseEvent(QMouseEvent * event)
{
	switch (OmStateManager::GetSystemMode()) {
	case NAVIGATION_SYSTEM_MODE:
		cameraMoving = false;
		PickToolGetColor(event);
		break;
	case DEND_MODE:
		break;
	case EDIT_SYSTEM_MODE:
		EditModeMouseRelease(event);
		break;
	}
}

void OmView2d::mouseMove_NavMode_CamMoving(QMouseEvent * event)
{
	Vector2i zoomMipVector = mViewGroupState->GetZoomLevel();
	Vector2f current_pan = GetPanDistance(mViewType);
	Vector2i drag = Vector2i((clickPoint.x - event->x()), clickPoint.y - event->y());
	Vector2i thisPoint = Vector2i(event->x(),event->y());

	if (OmLocalPreferences::getStickyCrosshairMode()) {
		SpaceCoord thisCoord = ScreenToSpaceCoord(thisPoint);
		SpaceCoord difference = thisCoord - rememberCoord;
	       	SpaceCoord depth = mViewGroupState->GetViewDepthCoord() - difference;
		mViewGroupState->SetViewSliceDepth(XY_VIEW, depth.z);
		mViewGroupState->SetViewSliceDepth(XZ_VIEW, depth.y);
		mViewGroupState->SetViewSliceDepth(YZ_VIEW, depth.x);
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
	} else {
		mViewGroupState->SetPanDistance(mViewType,current_pan - ScreenToPanShift(Vector2i(drag.x, drag.y)));
		SetViewSliceOnPan();
        	debug("cross","current_pan.x: %f current_pan.y %f \n", current_pan.x, current_pan.y);
		float xdepth,ydepth,zdepth;
		xdepth = mViewGroupState->GetViewSliceDepth(YZ_VIEW);
		ydepth = mViewGroupState->GetViewSliceDepth(XZ_VIEW);
		zdepth = mViewGroupState->GetViewSliceDepth(XY_VIEW);
		SpaceCoord mDepthCoord = Vector3f(xdepth,ydepth,zdepth);

		ScreenCoord crosshairCoord = SpaceToScreenCoord(mViewType, mDepthCoord);
		DataCoord crosshairdata = SpaceToDataCoord(mDepthCoord);
		debug("cross","crosshairdata.(x,y,z): (%i,%i,%i)\n",crosshairdata.x, crosshairdata.y, crosshairdata.z);
		debug("cross","crosshair.x: %i crosshair.y %i\n", crosshairCoord.x, crosshairCoord.y);
	}
	clickPoint.x = event->x();
	clickPoint.y = event->y();
}
//\}


/**
 * \name View Event Methods
 * \{
 */
void OmView2d::ViewBoxChangeEvent()
{
	myUpdate();
}

void OmView2d::ViewPosChangeEvent()
{
	myUpdate();
}

void OmView2d::ViewCenterChangeEvent()
{
	SpaceCoord depth;

	depth.x = mViewGroupState->GetViewSliceDepth(YZ_VIEW);
	depth.y = mViewGroupState->GetViewSliceDepth(XZ_VIEW);
	depth.z = mViewGroupState->GetViewSliceDepth(XY_VIEW);
	
	ScreenCoord crossCoord = SpaceToScreenCoord(mViewType, depth);
	ScreenCoord centerCoord= Vector2i(mTotalViewport.width/2,mTotalViewport.height/2);

        Vector2f currentPan = GetPanDistance(mViewType);
        Vector2f newPan = ScreenToPanShift(centerCoord - crossCoord);
        debug ("cross", "view: %i  newPan.(x,y): (%f,%f)\n", mViewType,newPan.x,newPan.y);
        newPan += currentPan;
        mViewGroupState->SetPanDistance(mViewType, newPan);
        debug ("cross","view: %i  depth.(x,y,z): (%f,%f,%f)\n",mViewType,depth.x,depth.y,depth.z);
        debug ("cross", "view: %i  currentPan.(x,y): (%f,%f)\n", mViewType,currentPan.x,currentPan.y);
        debug ("cross", "view: %i  newPan.(x,y): (%f,%f)\n", mViewType,newPan.x,newPan.y);
        debug ("cross", "view: %i  crossCoord.(x,y): (%i,%i)\n", mViewType,crossCoord.x,crossCoord.y);
        debug ("cross", "view: %i  centerCoord.(x,y): (%i,%i)\n", mViewType,centerCoord.x,centerCoord.y);

	SetViewSliceOnPan();

	myUpdate();
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}

void OmView2d::ViewRedrawEvent()
{
	myUpdate();
}

void OmView2d::VoxelModificationEvent(OmVoxelEvent * event)
{
	//voxels in a segmentation have been modified
	//valid methods: GetSegmentationId(), GetVoxels()

	if ((mVolumeType == SEGMENTATION) && (event->GetSegmentationId() == mImageId)) {

		set < DataCoord > modVoxels = event->GetVoxels();

		// these voxels are not flat, they are correct for ortho views

		set < DataCoord >::iterator itr;
		for (itr = modVoxels.begin(); itr != modVoxels.end(); itr++) {
			modifiedCoords.insert(*itr);
		}

		myUpdate();
	}
}

void OmView2d::SystemModeChangeEvent()
{
	if (mVolumeType == SEGMENTATION) {
		modified_Ids = OmProject::GetSegmentation(mImageId).GetSelectedSegmentIds();
		delete_dirty = true;
		myUpdate();
	}
}
//\}

/**
 * \name Key Event Methods
 * \{
 */
void OmView2d::keyPressEvent(QKeyEvent * event)
{
	//debug("genone","OmView2d::keyPressEvent -- " << mViewType);

	switch (event->key()) {
	case Qt::Key_D:
		mMIP = !mMIP;
		break;
	case Qt::Key_M:
		mEmitMovie = !mEmitMovie;
		break;
	case Qt::Key_P:
		setBrushToolDiameter();
		myUpdate();
		break;
	case Qt::Key_L:
		mLevelLock = !mLevelLock;
		myUpdate();
		break;
	case Qt::Key_F:
		// Toggle fill mode.
		myUpdate();
		break;
	case Qt::Key_Escape:
		resetWindow();
		break;
	case Qt::Key_Minus:
		{
			Vector2 < int >current_zoom = mViewGroupState->GetZoomLevel();

			if (!mLevelLock && (current_zoom.y == 6) && (current_zoom.x < mRootLevel)) {
				Vector2i new_zoom = Vector2 < int >(current_zoom.x + 1, 10);

				PanAndZoom(new_zoom);
			}

			else if (current_zoom.y > 1) {
				Vector2i new_zoom = Vector2 < int >(current_zoom.x, current_zoom.y - 1);
				PanAndZoom(new_zoom);
			}
		}
		break;
	case Qt::Key_Equal:
		{
			Vector2 < int >current_zoom = mViewGroupState->GetZoomLevel();

			if (!mLevelLock && (current_zoom.y == 10) && (current_zoom.x > 0)) {
				Vector2i new_zoom = Vector2 < int >(current_zoom.x - 1, 6);
				
			} else	{
				Vector2i new_zoom = Vector2 < int >(current_zoom.x, current_zoom.y + 1);
				PanAndZoom(new_zoom);
			}
		}
		break;
	case Qt::Key_Right:
		{
			Vector2f current_pan = GetPanDistance(mViewType);

			mViewGroupState->SetPanDistance(mViewType,
								   Vector2f(current_pan.x + 5, current_pan.y));

			SetViewSliceOnPan();

		}
		break;
	case Qt::Key_Left:
		{
			Vector2f current_pan = GetPanDistance(mViewType);

			mViewGroupState->SetPanDistance(mViewType,
								   Vector2f(current_pan.x - 5, current_pan.y));

			SetViewSliceOnPan();
		}
		break;
	case Qt::Key_Up:
		{
			Vector2 < int >current_pan = GetPanDistance(mViewType);

			mViewGroupState->SetPanDistance(mViewType,
								   Vector2 < int >(current_pan.x, current_pan.y + 5));

			SetViewSliceOnPan();
		}
		break;
	case Qt::Key_Down:
		{
			Vector2f current_pan = GetPanDistance(mViewType);

			mViewGroupState->SetPanDistance(mViewType,
							Vector2f(current_pan.x, current_pan.y - 5));

			SetViewSliceOnPan();
		}
		break;
	case Qt::Key_W:
	case Qt::Key_PageUp:
		MoveUpStackCloserToViewer();
		if (OmLocalPreferences::getStickyCrosshairMode()){
			OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
		}
		break;
	case Qt::Key_S:
	case Qt::Key_PageDown:
		MoveDownStackFartherFromViewer();
		if (OmLocalPreferences::getStickyCrosshairMode()){
			OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
		}
		break;

	default:
		QWidget::keyPressEvent(event);
	}
}

//\}

void OmView2d::resetWindow()
{
	SpaceCoord depth = GetVolume().NormToSpaceCoord( NormCoord(0.5, 0.5, 0.5));
	mViewGroupState->SetViewSliceDepth(YZ_VIEW, depth.x);
	mViewGroupState->SetViewSliceDepth(XZ_VIEW, depth.y);
	mViewGroupState->SetViewSliceDepth(XY_VIEW, depth.z);
	mViewGroupState->SetPanDistance(YZ_VIEW, Vector2f(0,0));
	mViewGroupState->SetPanDistance(XZ_VIEW, Vector2f(0,0));
	mViewGroupState->SetPanDistance(XY_VIEW, Vector2f(0,0));			
	if (OmLocalPreferences::getStickyCrosshairMode()){
		debug("cross","we made it to the great Escape!\n");
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
	}
		
	doRedraw();
}

void OmView2d::doFindAndSplitSegment(QMouseEvent * event )
{
#if 1
        OmId segmentationID = mImageId;
	OmId segmentID;
        if(SEGMENTATION != mVolumeType) {
                return;
        }

        DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);
       	OmSegmentation & segmentation = OmProject::GetSegmentation(segmentationID);

	if(DendToolBar::GetSplitMode(segmentationID, segmentID)) {
		assert(mImageId==segmentationID);
	        OmId segid = segmentation.GetVoxelSegmentId(globalDataClickPoint);

		OmSegment * seg1 = segmentation.GetSegment(segmentID);
		OmSegment * seg2 = segmentation.GetSegment(segid);

                if(NULL == seg1 || NULL == seg2) {
                        return;
                }

        	seg1->splitTwoChildren(seg2);

		DendToolBar::SetSplitMode(false);
        	OmStateManager::SetSystemModePrev();
	} else {
	        segmentID = segmentation.GetVoxelSegmentId(globalDataClickPoint);
		debug("split", "segmentID=%i\n", segmentID);
		if (segmentID && segmentation.GetSegment(segmentID)) {
				DendToolBar::SetSplitMode(segmentationID, segmentID);
		}
	}

#else
	SegmentDataWrapper * sdw = getSelectedSegment( event );
	if( NULL == sdw ){
		return;
	}

	if( SEGMENTATION != mVolumeType ){
		return;
	}

	OmSegment * seg1 = sdw->getSegment();

	DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);
	OmId segmentationID = mImageId;
	OmSegmentation & segmentation = OmProject::GetSegmentation(segmentationID);

	OmId segid = segmentation.GetVoxelSegmentId(globalDataClickPoint);
	OmSegment * seg2 = segmentation.GetSegment(segid);
		
	seg1->splitTwoChildren(seg2);

	OmStateManager::SetSystemModePrev();
#endif
}
