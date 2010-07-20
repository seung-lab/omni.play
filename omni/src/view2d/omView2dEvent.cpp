#include "project/omProject.h"
#include "segment/actions/omSegmentEditor.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/segment/omSegmentSplitAction.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"
#include "system/events/omView3dEvent.h"
#include "system/omEventManager.h"
#include "system/omLocalPreferences.h"
#include "system/omStateManager.h"
#include "system/viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "view2d/omView2d.h"
#include "volume/omSegmentation.h"

/**
 * \name Mouse Event Handlers 
 */

//\{
void OmView2d::mousePressEvent(QMouseEvent * event)
{
	clickPoint.x = event->x();
	clickPoint.y = event->y();

	if( SPLIT_MODE == OmStateManager::GetToolMode()){
		if (event->button() == Qt::LeftButton) {
			doFindAndSplitSegment( event );
			doRedraw();
			return;
		}
	}

	if( event->button() == Qt::LeftButton) {
		const bool crosshair = event->modifiers() & Qt::ControlModifier;
		if( crosshair ){
			mouseSetCrosshair(event);
		} else {
			mouseLeftButton(event);
		}
	} else if (event->button() == Qt::RightButton) {
		if(event->modifiers() & Qt::ControlModifier) {
			mouseSelectSegment(event);
		} else {
			mouseShowSegmentContextMenu(event);
		}
	}
			
	cameraMoving = true;
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

	mViewGroupState->setTool(PAN_MODE);
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

	if( !sdw.isValid() ){
		printf("not valid\n");
		return;
	}

	const OmId segmentID = sdw.getID();

	OmSegmentEditor::SetEditSelection( segmentation.GetId(), segmentID);

	OmSegmentSelector sel( segmentation.GetId(), this, "view2dEvent" );
	if( augment_selection ){
		sel.augmentSelectedSet_toggle( segmentID);
	} else {
		sel.selectJustThisSegment_toggle( segmentID );
	}
	sel.sendEvent();
	
	Refresh();
	mTextures.clear();
	myUpdate();
}

void OmView2d::mouseShowSegmentContextMenu(QMouseEvent * event)
{
	SegmentDataWrapper * sdw = getSelectedSegment(event);
	if(sdw) {
        	mSegmentContextMenu.Refresh(*sdw, mViewGroupState);
        	mSegmentContextMenu.exec(event->globalPos());
	}
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

			if (OmProject::IsSegmentationValid(filter.GetSegmentation()) ) {
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

void OmView2d::MouseRelease_LeftButton_Filling(QMouseEvent * event)
{
	mScribbling = false;
	DataCoord dataClickPoint = getMouseClickpointLocalDataCoord(event);
	DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);

	//store current selection
	SegmentDataWrapper sdw = OmSegmentEditor::GetEditSelection();

	//return if not valid
	if (!sdw.isValid() ){
		return;
	}

	//switch on tool mode
	OmSegID data_value;
	switch (OmStateManager::GetToolMode()) {
	case FILL_MODE:
		//get value associated to segment id
		data_value = sdw.getID();
		break;

	default:
		return;
		break;
	}

	const OmSegID segid = sdw.getSegmentation().GetVoxelSegmentId(globalDataClickPoint);
	if(!segid) {
		return;
	}
       	const OmSegID rootSegID = sdw.getSegmentation().GetSegmentCache()->findRootID(segid);
	FillToolFill( sdw.getSegmentationID(), globalDataClickPoint, data_value, rootSegID );

	doRedraw();
}

void OmView2d::MouseRelease(QMouseEvent * event)
{
	// END PAINTING

	bool doselection = false;

	if (event->button() == Qt::RightButton) {
		return;
	}

	if (event->button() == Qt::LeftButton) {
		if (amInFillMode()) {
			MouseRelease_LeftButton_Filling(event);
		} else if (mScribbling) {

			mScribbling = false;
			DataCoord dataClickPoint = getMouseClickpointLocalDataCoord(event);
			DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);

			//store current selection
			SegmentDataWrapper sdw = OmSegmentEditor::GetEditSelection();

			//return if not valid
			if (!sdw.isValid()){
				return;
			}

			//switch on tool mode
			OmSegID data_value;
			switch (OmStateManager::GetToolMode()) {
			case ADD_VOXEL_MODE:
				data_value = sdw.getID();
				break;

			case SUBTRACT_VOXEL_MODE:
				data_value = NULL_SEGMENT_VALUE;
				break;

			case SELECT_MODE:
				doselection = true;
				break;

			default:
				return;
				break;
			}

			//run action
			if (!doselection) {
				BrushToolApplyPaint(sdw.getSegmentationID(), globalDataClickPoint, data_value);
			} else {
				PickToolAddToSelection(sdw.getSegmentationID(), globalDataClickPoint);
			}

			lastDataPoint = dataClickPoint;

			myUpdate();
		}
	}
}

void OmView2d::MouseMove(QMouseEvent * event)
{
	// KEEP PAINTING
	
	if (PAN_MODE == OmStateManager::GetToolMode()) {
		mouseMove_CamMoving(event);
	} else if (mScribbling) {
		MouseMove_LeftButton_Scribbling(event);
		return;
	}
}
void OmView2d::wheelEvent(QWheelEvent * event)
{
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
	if (numSteps >= 0) { // ZOOMING IN

		Vector2 < int >current_zoom = mViewGroupState->GetZoomLevel();

		if (!mLevelLock && (current_zoom.y >= 10) && (current_zoom.x > 0)) {
			// need to move to previous mip level
			Vector2 <int> new_zoom = Vector2 < int >(current_zoom.x - 1, 6);
		        PanAndZoom(new_zoom);
		} else{
			Vector2 <int> new_zoom=Vector2i(current_zoom.x,current_zoom.y+ (1 * numSteps));
		        PanAndZoom(new_zoom);
		}
	} else { // ZOOMING OUT

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

void OmView2d::mouseLeftButton(QMouseEvent * event)
{
	debug ("view2d", "OmView2d::mouseLeftButton %i,%i\n", SELECT_MODE, OmStateManager::GetToolMode());
	bool doselection = false;
	bool dosubtract = false;
	
	OmSegID data_value;

	switch (OmStateManager::GetToolMode()) {
	case SELECT_MODE:
		mScribbling = true;
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
		mScribbling = true;
		break;
	case SUBTRACT_VOXEL_MODE:
		mScribbling = true;
		dosubtract = true;
		break;
	case SELECT_VOXEL_MODE:	
		return;
		break;
	default:
		return;
		break;
	}

	DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);
	SegmentDataWrapper sdw = OmSegmentEditor::GetEditSelection();
	if ( sdw.isValid() ) {
		//run action
		if (!doselection) {
			if (dosubtract) {
				data_value = NULL_SEGMENT_VALUE;
			} else {
				data_value = sdw.getID();
			}
			BrushToolApplyPaint(sdw.getSegmentationID(), globalDataClickPoint, data_value);
		} else {
			PickToolAddToSelection(sdw.getSegmentationID(), globalDataClickPoint);
			bresenhamLineDraw(lastDataPoint, globalDataClickPoint, true);
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
	if (event->buttons() == Qt::LeftButton) {
		switch (OmStateManager::GetToolMode()) {
		case SPLIT_MODE:
			break;

		case SELECT_MODE:
		case CROSSHAIR_MODE:
		case PAN_MODE:
		case ZOOM_MODE:
			if (cameraMoving && PAN_MODE == OmStateManager::GetToolMode()) {
				mouseMove_CamMoving(event);
				OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));
			} else if(cameraMoving && SELECT_MODE == OmStateManager::GetToolMode()){
                        	MouseMove_LeftButton_Scribbling(event);
                        }

			break;

		case ADD_VOXEL_MODE:
		case SUBTRACT_VOXEL_MODE:
		case FILL_MODE:
			MouseMove(event);
			break;

		case SELECT_VOXEL_MODE:
			assert(0 && "not implemented");
		}
	}

	myUpdate();
}

void OmView2d::MouseMove_LeftButton_Scribbling(QMouseEvent * event)
{
	bool doselection = false;

	DataCoord dataClickPoint = getMouseClickpointLocalDataCoord(event);
	DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);

	//store current selection
	SegmentDataWrapper sdw = OmSegmentEditor::GetEditSelection();

	//return if not valid
	if (!sdw.isValid())
		return;

	//switch on tool mode
	OmSegID data_value;
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		//get value associated to segment id
		data_value = sdw.getID();
		break;

	case SUBTRACT_VOXEL_MODE:
		data_value = NULL_SEGMENT_VALUE;
		break;

	case SELECT_MODE:
		doselection = true;
		break;

	default:
		return;
		break;
	}

	if (!doselection) {
		//run action
		BrushToolApplyPaint(sdw.getSegmentationID(), globalDataClickPoint, data_value);
		bresenhamLineDraw(lastDataPoint, dataClickPoint);
	} else {
		// TODO: bug here; ask MattW
		bresenhamLineDraw(lastDataPoint, dataClickPoint, doselection);
		PickToolAddToSelection(sdw.getSegmentationID(), globalDataClickPoint);
	}

	lastDataPoint = dataClickPoint;

	myUpdate();
}

void OmView2d::mouseReleaseEvent(QMouseEvent * event)
{
	mScribbling = false;

	switch (OmStateManager::GetToolMode()) {
	case SPLIT_MODE:
		break;

	case SELECT_MODE:
	case CROSSHAIR_MODE:
	case PAN_MODE:
	case ZOOM_MODE:
		cameraMoving = false;
		break;

	case ADD_VOXEL_MODE:
	case SUBTRACT_VOXEL_MODE:
	case FILL_MODE:
		MouseRelease(event);
		break;

	case SELECT_VOXEL_MODE:
		assert(0 && "not implemented");
		break;
	}
}

void OmView2d::mouseMove_CamMoving(QMouseEvent * event)
{
	Vector2i zoomMipVector = mViewGroupState->GetZoomLevel();
	Vector2f current_pan = GetPanDistance(mViewType);
	Vector2i drag = Vector2i((clickPoint.x - event->x()), clickPoint.y - event->y());

	SpaceCoord dragCoord = ScreenToSpaceCoord(drag);
	SpaceCoord zeroCoord = ScreenToSpaceCoord(Vector2i(0,0));

	//debug("jitterbug", "testCoord4: %f,%f,%f\n", DEBUGV3(testCoord4));
	//debug("jitterbug", "testCoord5: %f,%f,%f\n", DEBUGV3(testCoord5));

	if (OmLocalPreferences::getStickyCrosshairMode()) {
		SpaceCoord difference = zeroCoord - dragCoord;
	       	SpaceCoord oldDepth = mViewGroupState->GetViewDepthCoord();
	       	SpaceCoord depth = oldDepth - difference;
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

void OmView2d::resetWindowState()
{
	SpaceCoord depth = mVolume->NormToSpaceCoord( NormCoord(0.5, 0.5, 0.5));
	mViewGroupState->SetViewSliceDepth(YZ_VIEW, depth.x);
	mViewGroupState->SetViewSliceDepth(XZ_VIEW, depth.y);
	mViewGroupState->SetViewSliceDepth(XY_VIEW, depth.z);
	mViewGroupState->SetPanDistance(YZ_VIEW, Vector2f(0,0));
	mViewGroupState->SetPanDistance(XZ_VIEW, Vector2f(0,0));
	mViewGroupState->SetPanDistance(XY_VIEW, Vector2f(0,0));
}

void OmView2d::resetWindow()
{
	resetWindowState();

	if (OmLocalPreferences::getStickyCrosshairMode()){
		debug("cross","we made it to the great Escape!\n");
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
	}
		
	doRedraw();
}

void OmView2d::doFindAndSplitSegment(QMouseEvent * event )
{
	SegmentDataWrapper * sdw = getSelectedSegment( event );

        if(NULL == sdw) {
                return;
        }

        DataCoord globalDataClickPoint = getMouseClickpointGlobalDataCoord(event);
       	OmSegmentation & segmentation = sdw->getSegmentation();

	OmId segmentationID, segmentID;
	if(mViewGroupState->GetSplitMode(segmentationID, segmentID)) {
		assert(mImageId==segmentationID);
	        OmId segid = segmentation.GetVoxelSegmentId(globalDataClickPoint);

		OmSegment * seg1 = segmentation.GetSegmentCache()->GetSegment(segmentID);
		OmSegment * seg2 = segmentation.GetSegmentCache()->GetSegment(segid);

                if(NULL == seg1 || NULL == seg2) {
                        return;
                }

		OmSegmentSplitAction::RunIfSplittable(seg1, seg2);

		mViewGroupState->SetSplitMode(false);
	} else {
	        segmentID = segmentation.GetVoxelSegmentId(globalDataClickPoint);
		debug("split", "segmentID=%i\n", segmentID);
		if (segmentID && segmentation.GetSegmentCache()->GetSegment(segmentID)) {
				mViewGroupState->SetSplitMode(segmentationID, segmentID);
		}
	}

}
