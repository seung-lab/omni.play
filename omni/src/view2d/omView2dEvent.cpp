#include "omView2d.h"
#include "segment/actions/segment/omSegmentSelectionAction.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/voxel/omVoxelSetValueAction.h"
#include "segment/actions/segment/omSegmentMergeAction.h"
#include "segment/omSegmentEditor.h"
#include "segment/omSegment.h"
#include "system/omStateManager.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omLocalPreferences.h"
#include "system/omSystemTypes.h"
#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "volume/omVolume.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

#include "project/omProject.h"
        bool amInFillMode();
        bool doDisplayInformation();

/**
 * \name Mouse Event Handlers 
 */

//\{
void OmView2d::mouseDoubleClickEvent(QMouseEvent * event)
{
	switch (OmStateManager::GetSystemMode()) {
	case NAVIGATION_SYSTEM_MODE:
		break;
	case EDIT_SYSTEM_MODE:
		break;
	}
}

void OmView2d::mousePressEvent(QMouseEvent * event)
{
	clickPoint.x = event->x();
	clickPoint.y = event->y();
	rememberCoord = ScreenToSpaceCoord(mViewType, clickPoint);
	rememberDepthCoord = OmStateManager::GetViewDepthCoord();

	switch (OmStateManager::GetSystemMode()) {

		case NAVIGATION_SYSTEM_MODE: {
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
		}
		break;

		case EDIT_SYSTEM_MODE: {
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
		}
		break;
	}
}

void OmView2d::mouseSetCrosshair(QMouseEvent * event)
{
	Refresh();
	mTextures.clear();
	myUpdate();

	SetDepth(event);

	OmStateManager::SetToolMode(PAN_MODE);
	OmEventManager::PostEvent(new OmSystemModeEvent(OmSystemModeEvent::SYSTEM_MODE_CHANGE));
}

// XY_VIEW is the default viewType 
// (different newTypes only used by mouseMove_NavMode_DrawInfo(...) for some reason??? (purcaro)
DataCoord OmView2d::getMouseClickpointLocalDataCoord(QMouseEvent * event, const ViewType viewType)
{
	Vector2f clickPoint = Vector2f(event->x(), event->y());
	return ScreenToDataCoord(mViewType, clickPoint);
}

DataCoord OmView2d::getMouseClickpointGlobalDataCoord(QMouseEvent * event)
{

	return  getMouseClickpointLocalDataCoord(event);
}

void OmView2d::mouseSelectSegment(QMouseEvent * event)
{
	bool augment_selection = event->modifiers() & Qt::ShiftModifier;

	// find segment selected
	DataCoord dataClickPoint = getMouseClickpointGlobalDataCoord(event);

	OmId theId;
	OmId segmentationID;
	bool found = false;
	if (mVolumeType == SEGMENTATION) {
		found = true;
		OmSegmentation & current_seg = OmVolume::GetSegmentation(mImageId);
		segmentationID = mImageId;
		theId = current_seg.GetVoxelSegmentId(dataClickPoint);
	} else {
		OmChannel & current_channel = OmVolume::GetChannel(mImageId);
		const set < OmId > objectIDs = current_channel.GetValidFilterIds();
		set < OmId >::iterator obj_it;

		for( obj_it=objectIDs.begin(); obj_it != objectIDs.end(); obj_it++ ) {
			OmFilter2d &filter = current_channel.GetFilter(*obj_it);
			if (filter.GetSegmentation ()) {
				found = true;
				segmentationID = filter.GetSegmentation();
				OmSegmentation & current_seg = OmVolume::GetSegmentation(segmentationID);
				theId = current_seg.GetVoxelSegmentId(dataClickPoint);
				break;
			}
		}
	}
	if (!found) {
		return;
	}

	OmSegmentation & segmentation = OmVolume::GetSegmentation(segmentationID);

	if (segmentation.IsSegmentValid(theId)) {

		OmSegmentEditor::SetEditSelection(segmentationID, theId);

		const bool curSegmentNotYetMarkedAsSelected = !(segmentation.IsSegmentSelected(theId));

		// if not augmenting slection and selecting segment, then 
		//  select new segment, and deselect current segment(s)
		if (!augment_selection && curSegmentNotYetMarkedAsSelected) {
			OmIds select_segment_ids;
			select_segment_ids.insert(theId);
			(new OmSegmentSelectAction(segmentationID,
						   select_segment_ids,
						   segmentation.GetSelectedSegmentIds(), theId))->Run();
		} else {
			(new OmSegmentSelectAction(segmentationID,
						   theId, curSegmentNotYetMarkedAsSelected, theId))->Run();
		}
		Refresh();
		mTextures.clear();
		myUpdate();
	}
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
	SEGMENT_DATA_TYPE data_value;
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		//get value associated to segment id
		data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
		break;

	case SUBTRACT_VOXEL_MODE:
		data_value = NULL_SEGMENT_DATA;
		break;

	default:
		return;
		break;
	}

	OmId segid = OmVolume::GetSegmentation(segmentation_id).GetVoxelSegmentId(globalDataClickPoint);
	FillToolFill(segmentation_id, globalDataClickPoint, data_value, segid);
	Refresh();
	mTextures.clear();
	myUpdate();
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
			SEGMENT_DATA_TYPE data_value;
			switch (OmStateManager::GetToolMode()) {
			case ADD_VOXEL_MODE:
				//get value associated to segment id
				data_value =
				    OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
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

		Vector2 < int >current_zoom = OmStateManager::Instance()->GetZoomLevel();

		if (!mLevelLock && (current_zoom.y >= 10) && (current_zoom.x > 0)) {
			// need to move to previous mip level
			Vector2 <int> new_zoom = Vector2 < int >(current_zoom.x - 1, 6);
		        PanAndZoom(new_zoom);
		} else{
			//OmStateManager::Instance()->SetZoomLevel(Vector2 <
			//				 int >(current_zoom.x,
			//					       current_zoom.y + (1 * numSteps)));
			Vector2 <int> new_zoom=Vector2i(current_zoom.x,current_zoom.y+ (1 * numSteps));
		        PanAndZoom(new_zoom);
		}
	} else {
		// ZOOMING OUT

		Vector2 < int >current_zoom = OmStateManager::Instance()->GetZoomLevel();

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
	
	SEGMENT_DATA_TYPE data_value;

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
				data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
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
	SEGMENT_DATA_TYPE data_value;
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		//get value associated to segment id
		data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
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
	case EDIT_SYSTEM_MODE:
		EditModeMouseRelease(event);
		break;
	}
}

void OmView2d::mouseMove_NavMode_CamMoving(QMouseEvent * event)
{
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	Vector2f current_pan = GetPanDistance(mViewType);
	Vector2i drag = Vector2i((clickPoint.x - event->x()), clickPoint.y - event->y());
	Vector2i thisPoint = Vector2i(event->x(),event->y());

	if (OmLocalPreferences::getStickyCrosshairMode()) {
		SpaceCoord thisCoord = ScreenToSpaceCoord(mViewType,thisPoint);
		SpaceCoord difference = thisCoord - rememberCoord;
	       	SpaceCoord depth = OmStateManager::GetViewDepthCoord() - difference;
		OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, depth.z);
		OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, depth.y);
		OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, depth.x);
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
	} else {
		OmStateManager::Instance()->SetPanDistance(mViewType,current_pan - ScreenToPanShift(Vector2i(drag.x, drag.y)));
		SetViewSliceOnPan();
        	debug("cross","current_pan.x: %f current_pan.y %f \n", current_pan.x, current_pan.y);
		float xdepth,ydepth,zdepth;
		xdepth = OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW);
		ydepth = OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW);
		zdepth = OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW);
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
void OmView2d::ViewBoxChangeEvent(OmViewEvent * event)
{
	//debug("genone","OmView2d::ViewBoxChangeEvent -- " << mViewType);
	myUpdate();
}

void OmView2d::ViewPosChangeEvent(OmViewEvent * event)
{
	myUpdate();
}

void OmView2d::ViewCenterChangeEvent(OmViewEvent * event)
{
	SpaceCoord depth;

	depth.x = OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW);
	depth.y = OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW);
	depth.z = OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW);
	
	ScreenCoord crossCoord = SpaceToScreenCoord(mViewType, depth);
	//crossCoord =  SpaceToScreenCoord(mViewType, ScreenToSpaceCoord(mViewType, crossCoord));
	ScreenCoord centerCoord= Vector2i(mTotalViewport.width/2,mTotalViewport.height/2);

        Vector2f currentPan = GetPanDistance(mViewType);
        Vector2f newPan = ScreenToPanShift(centerCoord - crossCoord);
        debug ("cross", "view: %i  newPan.(x,y): (%f,%f)\n", mViewType,newPan.x,newPan.y);
        newPan += currentPan;
        OmStateManager::Instance()->SetPanDistance(mViewType, newPan);
        debug ("cross","view: %i  depth.(x,y,z): (%f,%f,%f)\n",mViewType,depth.x,depth.y,depth.z);
        debug ("cross", "view: %i  currentPan.(x,y): (%f,%f)\n", mViewType,currentPan.x,currentPan.y);
        debug ("cross", "view: %i  newPan.(x,y): (%f,%f)\n", mViewType,newPan.x,newPan.y);
        debug ("cross", "view: %i  crossCoord.(x,y): (%i,%i)\n", mViewType,crossCoord.x,crossCoord.y);
        debug ("cross", "view: %i  centerCoord.(x,y): (%i,%i)\n", mViewType,centerCoord.x,centerCoord.y);

	SetViewSliceOnPan();

	myUpdate();
	OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
}

void OmView2d::ViewRedrawEvent(OmViewEvent * event)
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
			//debug("genone","data coord = " << *itr);
			modifiedCoords.insert(*itr);

		}

		myUpdate();
	}
}

void OmView2d::SystemModeChangeEvent(OmSystemModeEvent * event)
{
	//debug("genone","OmView2d::SystemModeChangeEvent");

	if (mVolumeType == SEGMENTATION) {
		modified_Ids = OmVolume::GetSegmentation(mImageId).GetSelectedSegmentIds();
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
		{
			mMIP = !mMIP;
		}
		break;
	case Qt::Key_M:
		{
			mEmitMovie = !mEmitMovie;
		}
		break;
	case Qt::Key_P:
		{
			setBrushToolDiameter();
			myUpdate();
		}
		break;
	case Qt::Key_L:
		{
			mLevelLock = !mLevelLock;
			myUpdate();
		}
		break;
	case Qt::Key_J:
		// Toggle joining of a segment.
		{
			bool domerge = false;
			OmId seg;
			if (mVolumeType == SEGMENTATION) {
				domerge = true;
				seg = mImageId;
			} else {
				OmChannel & current_channel = OmVolume::GetChannel(mImageId);
				const set < OmId > objectIDs = current_channel.GetValidFilterIds();
				set < OmId >::iterator obj_it;

                		for( obj_it=objectIDs.begin(); obj_it != objectIDs.end(); obj_it++ ) {
                        		OmFilter2d &filter = current_channel.GetFilter(*obj_it);
                        		seg = filter.GetSegmentation ();
                        		if (seg) {
						domerge = true;
						break;
					}
				}
			}

			if (domerge) {
				OmSegmentation & current_seg = OmVolume::GetSegmentation(seg);
				(new OmSegmentMergeAction(seg))->Run();
				current_seg.SetAllSegmentsSelected(false);
				Refresh();
				mTextures.clear();
			}

			myUpdate();
		}
		break;

	case Qt::Key_F:
		// Toggle fill mode.
		{
			myUpdate();
		}
		break;
	case Qt::Key_Escape:
		{
			
			SpaceCoord depth = OmVolume::NormToSpaceCoord( NormCoord(0.5, 0.5, 0.5));
			OmStateManager::SetViewSliceDepth(YZ_VIEW, depth.x);
			OmStateManager::SetViewSliceDepth(XZ_VIEW, depth.y);
			OmStateManager::SetViewSliceDepth(XY_VIEW, depth.z);
			OmStateManager::SetPanDistance(YZ_VIEW, Vector2f(0,0));
			OmStateManager::SetPanDistance(XZ_VIEW, Vector2f(0,0));
			OmStateManager::SetPanDistance(XY_VIEW, Vector2f(0,0));			
			if (OmLocalPreferences::getStickyCrosshairMode()){
				debug("cross","we made it to the great Escape!\n");
				OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
			}
		
			Refresh();
			mTextures.clear();
			myUpdate();

		}
		break;
	case Qt::Key_Minus:
		{
			Vector2 < int >current_zoom = OmStateManager::Instance()->GetZoomLevel();

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
			Vector2 < int >current_zoom = OmStateManager::Instance()->GetZoomLevel();

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

			OmStateManager::Instance()->SetPanDistance(mViewType,
								   Vector2f(current_pan.x + 5, current_pan.y));

			SetViewSliceOnPan();

		}
		break;
	case Qt::Key_Left:
		{
			Vector2f current_pan = GetPanDistance(mViewType);

			OmStateManager::Instance()->SetPanDistance(mViewType,
								   Vector2f(current_pan.x - 5, current_pan.y));

			SetViewSliceOnPan();
		}
		break;
	case Qt::Key_Up:
		{
			Vector2 < int >current_pan = GetPanDistance(mViewType);

			OmStateManager::Instance()->SetPanDistance(mViewType,
								   Vector2 < int >(current_pan.x, current_pan.y + 5));

			SetViewSliceOnPan();
		}
		break;
	case Qt::Key_Down:
		{
			Vector2f current_pan = GetPanDistance(mViewType);

			OmStateManager::Instance()->SetPanDistance(mViewType,
								   Vector2f(current_pan.x, current_pan.y - 5));

			SetViewSliceOnPan();
		}
		break;
	case Qt::Key_W:
	case Qt::Key_PageUp:
		{
			MoveUpStackCloserToViewer();
			if (OmLocalPreferences::getStickyCrosshairMode()){
				OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
			}
		}
		break;
	case Qt::Key_S:
	case Qt::Key_PageDown:
		{
			MoveDownStackFartherFromViewer();
			if (OmLocalPreferences::getStickyCrosshairMode()){
				OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
			}
		}
		break;

	default:
		QWidget::keyPressEvent(event);
	}
}

//\}
