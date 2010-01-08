#include "domView2d.h"

#include "omCachingTile.h"
#include "omTextureID.h"

#include "system/omStateManager.h"
#include "system/omProject.h"
#include "system/omSystemTypes.h"
#include "system/omEventManager.h"

#include "segment/omSegment.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

#include "segment/actions/segment/omSegmentSelectionAction.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/voxel/omVoxelSetValueAction.h"

#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include "segment/omSegmentEditor.h"
#include "volume/omMipChunk.h"

#include "common/omGl.h"

#include <boost/progress.hpp>

#include <QPainter>

#include <QPrinter>
#include <QGLFramebufferObject>

#define DEBUG 0

class myQGLWidget: public QGLWidget
{
public:
	void initializeGL();
	void paintGL();
	myQGLWidget ();
};
myQGLWidget::myQGLWidget () :QGLWidget()
{
	makeCurrent ();
}
void myQGLWidget::initializeGL()
{
	std::cerr << "in  myQGLWidget::" << __FUNCTION__ << endl;
	static float c=0.0;
        c+=.01;
        if (c> 1.0) c = 0.0;
        glClearColor (c, c, 0.0f, 0.0f);
}
void myQGLWidget::paintGL()
{
	std::cerr << "in  myQGLWidget::" << __FUNCTION__ << endl;
	makeCurrent ();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	doneCurrent ();
}

static myQGLWidget * h=NULL;

DataBbox SpaceToDataBbox(const SpaceBbox &spacebox)
{
        DataBbox new_data_box;
        new_data_box = OmVolume::NormToDataBbox(OmVolume::SpaceToNormBbox(spacebox));
        return new_data_box;
}
SpaceBbox DataToSpaceBbox(const DataBbox &databox)
{
       	SpaceBbox new_space_box;
       	new_space_box = OmVolume::NormToSpaceBbox(OmVolume::DataToNormBbox(databox));
        return new_space_box;
}
DataCoord SpaceToDataCoord(const SpaceCoord &spacec)
{
	DataCoord new_data_center;
        new_data_center = OmVolume::NormToDataCoord(OmVolume::SpaceToNormCoord(spacec));
        return new_data_center;
}
SpaceCoord DataToSpaceCoord(const DataCoord &datac)
{
        SpaceCoord new_space_center;
      	new_space_center = OmVolume::NormToSpaceCoord(OmVolume::DataToNormCoord(datac));
        return new_space_center;
}

void DOMNIView2d::ViewBoxChangeEvent(OmViewEvent *event) {
        std::cerr << __FUNCTION__ << endl;
        //return;
        DOUT("OmView2d::ViewBoxChangeEvent -- " << mViewType);
        update();
}
void DOMNIView2d::ViewPosChangeEvent(OmViewEvent *event) {
        std::cerr << __FUNCTION__ << endl;
        //return;
        DOUT("OmView2d::ViewPosChangeEvent -- " << mViewType);
        update();

}
void DOMNIView2d::ViewCenterChangeEvent(OmViewEvent *event) {
        std::cerr << __FUNCTION__ << endl;
        //return;
        DOUT("OmView2d::ViewCenterChangeEvent");

        // update view center
        update();
}
void DOMNIView2d::ViewRedrawEvent(OmViewEvent *event) {
        std::cerr << __FUNCTION__ << endl;
        //return;
        //DOUT("ViewRedrawEvent");
        update();
}
void DOMNIView2d::EditModeMousePressed(QMouseEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	// VoxelEditMouse(event, false);
	// START PAINTING
	
	if (event->button() == Qt::LeftButton) {
		
		Vector2f clickPoint = Vector2f(event->x(), event->y());

		int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
		int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
		float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
		
		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
		float scaleFactor = (zoomMipVector.y / 10.0);
		
		Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
		
		DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
		int mViewDepth = data_coord.z;
		
		DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
		//cout << "mouse pressed: " << dataClickPoint << endl;
		scribbling = true;
		
		
		// modifiedCoords.clear();

		// okay, dataClickPoint is flat, only valid in XY ortho view.  This is not correct.  Needs to be global voxel coordinate.
		DataCoord globalDataClickPoint;
		switch(mViewType) {
			case XY_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.y, dataClickPoint.z);
				//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
			}
				break;
			case XZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.z, dataClickPoint.y);
			}
				break;
			case YZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.z, dataClickPoint.y, dataClickPoint.x);
			}
				break;
		}
		//cout << "global click point: " << globalDataClickPoint << endl;
		
		//store current selection
		OmId segmentation_id, segment_id;
		bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);
		
		//return if not valid
		if(!valid_edit_selection) return;
		
		//switch on tool mode
		SEGMENT_DATA_TYPE data_value;
		switch(OmStateManager::GetToolMode()) {
			case ADD_VOXEL_MODE:
				//get value associated to segment id
				data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
				break;
				
			case SUBTRACT_VOXEL_MODE:
				data_value = NULL_SEGMENT_DATA;
				break;
				
			default:
				assert(false);
		}
		
		//run action
		(new OmVoxelSetValueAction(segmentation_id, globalDataClickPoint, data_value))->Run();
		
		lastDataPoint = dataClickPoint;
	}
}
void DOMNIView2d::EditModeMouseRelease(QMouseEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	// END PAINTING
	if (event->button() == Qt::LeftButton && scribbling) {
		Vector2f clickPoint = Vector2f(event->x(), event->y());
		
		
		int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
		int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
		float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
		
		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
		float scaleFactor = (zoomMipVector.y / 10.0);
		
		Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
		
		DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
		int mViewDepth = data_coord.z;
		
		DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
		//drawLineTo(QPoint(clickPoint.x, clickPoint.y));
		scribbling = false;

		// okay, dataClickPoint is flat, only valid in XY ortho view.  This is not correct.  Needs to be global voxel coordinate.
		DataCoord globalDataClickPoint;
		switch(mViewType) {
			case XY_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.y, dataClickPoint.z);
				//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
			}
				break;
			case XZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.z, dataClickPoint.y);
			}
				break;
			case YZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.z, dataClickPoint.y, dataClickPoint.x);
			}
				break;
		}
		//cout << "global click point: " << globalDataClickPoint << endl;
		
		//store current selection
		OmId segmentation_id, segment_id;
		bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);
		
		//return if not valid
		if(!valid_edit_selection) return;
		
		//switch on tool mode
		SEGMENT_DATA_TYPE data_value;
		switch(OmStateManager::GetToolMode()) {
			case ADD_VOXEL_MODE:
				//get value associated to segment id
				data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
				break;
				
			case SUBTRACT_VOXEL_MODE:
				data_value = NULL_SEGMENT_DATA;
				break;
				
			default:
				assert(false);
		}
		
		//run action
		(new OmVoxelSetValueAction(segmentation_id, globalDataClickPoint, data_value))->Run();
		
		lastDataPoint = dataClickPoint;
	}
}
void DOMNIView2d::EditModeMouseMove(QMouseEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	// KEEP PAINTING
	DOUT("OmView2d::EditModeMouseMove");
	
	if ((event->buttons() & Qt::LeftButton) && scribbling) {
		Vector2f clickPoint = Vector2f(event->x(), event->y());
		
		
		int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
		int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
		float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
		
		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
		float scaleFactor = (zoomMipVector.y / 10.0);
		
		Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
		
		DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
		int mViewDepth = data_coord.z;
		DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
		
		// okay, dataClickPoint is flat, only valid in XY ortho view.  This is not correct.  Needs to be global voxel coordinate.
		DataCoord globalDataClickPoint;
		switch(mViewType) {
			case XY_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.y, dataClickPoint.z);
				//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
			}
				break;
			case XZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.z, dataClickPoint.y);
			}
				break;
			case YZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.z, dataClickPoint.y, dataClickPoint.x);
			}
				break;
		}
		//cout << "global click point: " << globalDataClickPoint << endl;

		
		//store current selection
		OmId segmentation_id, segment_id;
		bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);
		
		//return if not valid
		if(!valid_edit_selection) return;
		
		//switch on tool mode
		SEGMENT_DATA_TYPE data_value;
		switch(OmStateManager::GetToolMode()) {
			case ADD_VOXEL_MODE:
				//get value associated to segment id
				data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
				break;
				
			case SUBTRACT_VOXEL_MODE:
				data_value = NULL_SEGMENT_DATA;
				break;
				
			default:
				assert(false);
		}
		
		//run action
		(new OmVoxelSetValueAction(segmentation_id, globalDataClickPoint, data_value))->Run();
		
		//bresenhamLineDraw(lastDataPoint, dataClickPoint);
		
		lastDataPoint = dataClickPoint;
	}
	update ();	
}




void DOMNIView2d::resizeEvent (QResizeEvent * event)
{
        //std::cerr << " DOMNIView2d::" << __FUNCTION__ << this << endl;
	delete pbuffer;
	pbuffer = new QGLPixelBuffer(event->size(), QGLFormat::defaultFormat(), h);

        mTotalViewport.lowerLeftX = 0;
        mTotalViewport.lowerLeftY = 0;
        mTotalViewport.width = size().width();
        mTotalViewport.height = size().height();

        mNearClip = -1;
        mFarClip = 1;
}
void DOMNIView2d::mousePressEvent(QMouseEvent *event) {
        std::cerr << __FUNCTION__ << endl;
        //return;

        switch(OmStateManager::GetSystemMode()) {
                case NAVIGATION_SYSTEM_MODE: {
                        clickPoint.x = event->x();
                        clickPoint.y = event->y();

                        cameraMoving = true;
                }
                        break;

                case EDIT_SYSTEM_MODE: {
                        DOUT("edit mode mouse pressed")
                        EditModeMousePressed(event);
                }
                        break;
        }
}
void DOMNIView2d::mouseReleaseEvent(QMouseEvent *event) {
        std::cerr << __FUNCTION__ << endl;
        //return;

        switch(OmStateManager::GetSystemMode()) {
                case NAVIGATION_SYSTEM_MODE:
                        cameraMoving = false;
                        break;

                case EDIT_SYSTEM_MODE: {
                        DOUT("edit mode mouse released");
                        EditModeMouseRelease(event);
                }
                        break;
        }

        // signal ViewBoxChangeEvent
}
void DOMNIView2d::mouseMoveEvent(QMouseEvent *event) {
	//return;
	
	switch(OmStateManager::GetSystemMode()) {
		case NAVIGATION_SYSTEM_MODE: {
			
			if(cameraMoving) {
				
				Vector2<float> drag = Vector2<float>((clickPoint.x - event->x()), clickPoint.y - event->y());
				Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
				
				OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<float>(current_pan.x - drag.x, current_pan.y - drag.y));
				
				SpaceCoord panSpaceCoord = DataToSpaceCoord(DataCoord(drag.x, drag.y, 0));
				DOUT("panSpaceCoord = " << panSpaceCoord);
				if(mViewType == YZ_VIEW) {
					Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
					OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x + panSpaceCoord.y, slicemin.y + panSpaceCoord.x));
					
					Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
					OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x + panSpaceCoord.y, slicemax.y + panSpaceCoord.x));
				}
				else {
					
					Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
					OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x + panSpaceCoord.x, slicemin.y + panSpaceCoord.y));
					
					Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
					OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x + panSpaceCoord.x, slicemax.y + panSpaceCoord.y));
				}
				
				clickPoint.x = event->x();
				clickPoint.y = event->y();
				
			}
			
			else if(drawInformation) {
				Vector2f clickPoint = Vector2f(event->x(), event->y());
				
				int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
				int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
				float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
				
				Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
				float scaleFactor = (zoomMipVector.y / 10.0);
				
				Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
				
				DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
				
				int mViewDepth = data_coord.z;
				
				switch(mViewType) {
					case XY_VIEW: {
						DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
						
						
						if (mVolumeType == SEGMENTATION) {
							OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
							mSegmentID = current_seg.GetVoxelSegmentId(dataClickPoint);
							if(mSegmentID)
								mSegmentValue = current_seg.GetValueMappedToSegmentId(mSegmentID);
							else
								mSegmentValue = 0;
							// DOUT("segment ID = " << mSegmentID);
						}
					}
						break;
					case XZ_VIEW: {
						DataCoord dataClickPoint = DataCoord(localClickPoint.x, mViewDepth, localClickPoint.y);
						
						
						if (mVolumeType == SEGMENTATION) {
							OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
							mSegmentID = current_seg.GetVoxelSegmentId(dataClickPoint);
							if(mSegmentID)
								mSegmentValue = current_seg.GetValueMappedToSegmentId(mSegmentID);
							else
								mSegmentValue = 0;
							// DOUT("segment ID = " << mSegmentID);
						}
					}
						break;
					case YZ_VIEW: {
						DataCoord dataClickPoint = DataCoord(mViewDepth, localClickPoint.y, localClickPoint.x);
						
						
						if (mVolumeType == SEGMENTATION) {
							OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
							mSegmentID = current_seg.GetVoxelSegmentId(dataClickPoint);
							if(mSegmentID)
								mSegmentValue = current_seg.GetValueMappedToSegmentId(mSegmentID);
							else
								mSegmentValue = 0;
							// DOUT("segment ID = " << mSegmentID);
						}
					}
				}
				informationUpdated = true;
				update();
			}
			
			
		}
			break;
			
		case EDIT_SYSTEM_MODE: {
			if(!event->buttons()) return;
			EditModeMouseMove(event);
			DOUT("Moving mouse in edit mode");
		}
			break;
	}
}
void DOMNIView2d::keyPressEvent(QKeyEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	DOUT("OmView2d::keyPressEvent -- " << mViewType);
	
	
	switch (event->key()) {
		case Qt::Key_Minus:
		{
			Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
			
			if((current_zoom.y == 6) && (current_zoom.x < mRootLevel)) {
				OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x + 1, 10));
			}
			
			else if(current_zoom.y > 1)
				OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x, current_zoom.y - 1));
		}
			break;
		case Qt::Key_Equal:
		{
			Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
			
			if((current_zoom.y == 10) && (current_zoom.x > 0)) {
				OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x - 1, 6));
			}
			else
				OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x, current_zoom.y + 1));
		}
			break;
		case Qt::Key_Right:
		{
			Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
			
			OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x + 5, current_pan.y));
			
			SpaceCoord panSpaceCoord = DataToSpaceCoord(DataCoord(5, 0, 0));
			if(mViewType == YZ_VIEW) {
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x, slicemin.y - panSpaceCoord.x));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x, slicemax.y - panSpaceCoord.x));
			}
			else {
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x - panSpaceCoord.x, slicemin.y));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x - panSpaceCoord.x, slicemax.y));
			}
			
		}
			break;
		case Qt::Key_Left:
		{
			Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
			
			OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x - 5, current_pan.y));
			
			SpaceCoord panSpaceCoord = DataToSpaceCoord(DataCoord(5, 0, 0));
			if(mViewType == YZ_VIEW) {
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x, slicemin.y + panSpaceCoord.x));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x, slicemax.y + panSpaceCoord.x));
			}
			else {
				
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x + panSpaceCoord.x, slicemin.y));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x + panSpaceCoord.x, slicemax.y));
			}
		}
			break;
		case Qt::Key_Up:
		{
			Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
			
			OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x, current_pan.y + 5));
			
			SpaceCoord panSpaceCoord = DataToSpaceCoord(DataCoord(5, 0, 0));
			if(mViewType == YZ_VIEW) {
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x - panSpaceCoord.x, slicemin.y));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x - panSpaceCoord.x, slicemax.y));
			}
			else {
				
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x, slicemin.y - panSpaceCoord.x));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x, slicemax.y - panSpaceCoord.x));
			}
		}
			break;
		case Qt::Key_Down:
		{
			Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
			
			OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x, current_pan.y - 5));
			
			SpaceCoord panSpaceCoord = DataToSpaceCoord(DataCoord(5, 0, 0));
			if(mViewType == YZ_VIEW) {
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x + panSpaceCoord.x, slicemin.y));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x + panSpaceCoord.x, slicemax.y));
			}
			else {
				
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x, slicemin.y + panSpaceCoord.x));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x, slicemax.y + panSpaceCoord.x));
			}
		}
			break;
		case Qt::Key_W:
			// MOVE UP THE STACK, CLOSER TO VIEWER
		{
			float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
			DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
			
			int mViewDepth = data_coord.z;
			
			SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, mViewDepth + 1));
			
			OmStateManager::Instance()->SetViewSliceDepth(mViewType, space_coord.z);
		}
			break;
		case Qt::Key_S:
			// MOVE DOWN THE STACK, FARTHER FROM VIEWER
		{
			float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
			DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
			
			int mViewDepth = data_coord.z;
			
			SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, mViewDepth - 1));
			
			OmStateManager::Instance()->SetViewSliceDepth(mViewType, space_coord.z);
			
		}
			break;
		default:
			break;
			//QGLWidget::keyPressEvent(event);
	}
	update ();
}

void DOMNIView2d::wheelEvent ( QWheelEvent * event ) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	DOUT("OmView2d::wheelEvent -- " << mViewType);
	
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;
	
	
	if(numSteps >= 0) {
		// ZOOMING IN
		
		Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
		
		if((current_zoom.y == 10) && (current_zoom.x > 0)) {
			// need to move to previous mip level
			OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x - 1, 6));

                        float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType)*2;
                        DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
                        int mViewDepth = data_coord.z;
                        SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, mViewDepth));
                        OmStateManager::Instance()->SetViewSliceDepth(mViewType, space_coord.z);

		}
		else
			OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x, current_zoom.y + (1 * numSteps)));
	}
	else {
		// ZOOMING OUT
		
		Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
		
		if((current_zoom.y == 6) && (current_zoom.x < mRootLevel)) {
			// need to move to next mip level
			OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x + 1, 10));

                        float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType)/2;
                        DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
                        int mViewDepth = data_coord.z;
                        SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, mViewDepth));
                        OmStateManager::Instance()->SetViewSliceDepth(mViewType, space_coord.z);
		}
		
		else if(current_zoom.y > 1)
			OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x, current_zoom.y - (1 * (-1 * numSteps))));
	}
	
	event->accept();
}


void DOMNIView2d::paintCrosshairs(QPainter &w)
{
	int len = 10;
	int midx, midy;

	// Put a plus (+) right in the middle of the viewport.
	midx = mTotalViewport.width / 2;
	midy = mTotalViewport.height / 2;
	w.drawLine (midx,midy-len,midx,midy+len);
	w.drawLine (midx-len,midy,midx+len,midy);
}

bool OMNIView2dUpdatePlan::operator!=( const OMNIView2dUpdatePlan& rhs ) const
{
        return TRUE;
}
bool OMNIView2dUpdatePlan::operator==( const OMNIView2dUpdatePlan& rhs ) const
{
        return FALSE;
}
OMNIView2dUpdatePlan::OMNIView2dUpdatePlan()
{
}

OMNIView2dUpdatePlan DOMNIView2d::constructPlan ()
{
	OMNIView2dUpdatePlan plan;

        float depth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
        DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, depth));
        int viewDepth = data_coord.z;
	Vector2<int> curzoom = OmStateManager::Instance()->GetZoomLevel();
	Vector2i translateVector = OmStateManager::Instance()->GetPanDistance(mViewType);
	int x, y, z;
	OmDataVolumePlane plane;
        switch(mViewType) {
                case XY_VIEW:
			x = mDataCenter.x + translateVector.x;
			y = mDataCenter.y + translateVector.y;
			z = viewDepth;
			plane = VOL_XY_PLANE;
                        break;
                case XZ_VIEW:
			x = mDataCenter.x + translateVector.x;
			y = viewDepth;
			z = mDataCenter.y + translateVector.y;
			plane = VOL_XZ_PLANE;
                        break;
                case YZ_VIEW:
			x = viewDepth;
			y = mDataCenter.x + translateVector.x;
			z = mDataCenter.y + translateVector.y;
			plane = VOL_YZ_PLANE;
                        break;
        }
	int offx = x - mTotalViewport.width / 2;
	int offy = y - mTotalViewport.height / 2;
	int offz = z;
	int chunkcountx = offx / mTilelen;
	int chunkcounty = offy / mTilelen;
	int chunkcountz = offz / mTilelen;
	int chunkx = chunkcountx * mTilelen;
	int chunky = chunkcounty * mTilelen;
	int chunkz = chunkcountz * mTilelen;

	DataCoord dc = DataCoord (chunkx, chunky, chunkz);
	OmMipChunkCoord cc = mVolume->NormToMipCoord (OmVolume::SpaceToNormCoord (DataToSpaceCoord (dc)), curzoom.x);
	shared_ptr<OmMipChunk> chunk = mVolume->GetChunk (cc);
	Vector2<int> sliceDims;


	cout << "mViewType: " << mViewType << endl;
	cout << "width: " << mTotalViewport.width << endl;
	cout << "height: " << mTotalViewport.height << endl;
	cout << "depth: " << depth << endl;
	cout << "viewDepth: " << viewDepth << endl;
	cout << "miplevel: " << curzoom.x << endl;
	cout << "zoomlevel: " << curzoom.y << endl;
	cout << "translatex: " << translateVector.x << endl;
	cout << "translatey: " << translateVector.y << endl;
	cout << "mTilelen: " << mTilelen << endl;
	cout << "x center: " << mDataCenter.x << endl;
	cout << "y center: " << mDataCenter.y << endl;
	cout << "z center: " << mDataCenter.z << endl;
	cout << "x: " << x << endl;
	cout << "y: " << y << endl;
	cout << "z: " << z << endl;
	cout << "offx: " << offx << endl;
	cout << "offy: " << offy << endl;
	cout << "offz: " << offz << endl;
	cout << "chunkcountx: " << chunkcountx << endl;
	cout << "chunkcounty: " << chunkcounty << endl;
	cout << "chunkcountz: " << chunkcountz << endl;
	cout << "chunkx: " << chunkx << endl;
	cout << "chunky: " << chunky << endl;
	cout << "chunkz: " << chunkz << endl;
	cout << "dc: " << dc << endl;
	cout << "cc: " << cc << endl;

	cout << "chunk: " << chunk->GetCoordinate() << endl;
	void * void_data = chunk->ExtractDataSlice(plane, viewDepth, sliceDims);

	
	return plan;
}


QImage DOMNIView2d::paintTextures (OMNIView2dUpdatePlan &plan)
{
	static float c = 0.4;
	static float flip = -1;

	/////////////////////////////////////////////////////////////////////////////////
	// GL START CALLS HERE
	/////////////////////////////////////////////////////////////////////////////////
	
	pbuffer->makeCurrent();
#if 1
	flip *= -1;
        c += .4 * flip;
        if (c > 1.0) c = 0.0;
        glClearColor (c, c, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
	pbuffer->doneCurrent();

	/////////////////////////////////////////////////////////////////////////////////
	// GL END CALLS HERE
	/////////////////////////////////////////////////////////////////////////////////

	return pbuffer->toImage();
}

void showElapsed(double end, int iter, const std::string & name)
{
  static double totaltime = 0;
  double usec = end;
  totaltime += end;
  std::cout << name << ": " 
            << usec*1e3 << " nsec, total: " << totaltime  << " called times: " << iter << std::endl
            << "average: " << totaltime / iter
            << std::endl;
}

void DOMNIView2d::paintEvent(QPaintEvent *event)
{
	//std::cerr << "in  DOMNIView2d::" << __FUNCTION__ << endl;
	boost::timer t;		// Keep at top so it measures as much as possible.
	QImage m;

	QPainter w;
	w.begin (this);

	OMNIView2dUpdatePlan plan = constructPlan ();
	if (plan != mOldPlan) {
		m = paintTextures (plan);
		w.drawImage (QPoint (0,0), m);
	}
	
	paintCrosshairs (w);

	mOldPlan = plan;	// Keep mOldPlan as long as possible.

#if 1	// Display most of the time taken to do this paint event. Missing .end()
	QString str = QString::number (t.elapsed(), 'f', 4) + QString (" ms");
	w.drawText (QPoint (0,mTotalViewport.height), str);
#endif
	w.end();
}


DOMNIView2d::DOMNIView2d(ViewType viewtype, ObjectType voltype, OmId image_id, OmId second_id, OmId third_id, QWidget *parent)
: QWidget(parent)
{
	std::cerr << "in  DOMNIView2d::" << __FUNCTION__ << endl;

	if (!h) h = new myQGLWidget ();
        pbuffer = new QGLPixelBuffer(size(), QGLFormat::defaultFormat(), h);

	setFocusPolicy(Qt::ClickFocus);		// necessary for receiving keyboard events
	setMouseTracking(true);				// necessary for mouse-centered zooming
	setAutoFillBackground(false);		// necessary for proper QPainter functionality

	mVolxtent = &OmVolume::Instance ()->GetDataExtent();
	mDataCenter.x = mVolxtent->getMax().x/2;
	mDataCenter.y = mVolxtent->getMax().y/2;
	mDataCenter.z = mVolxtent->getMax().z/2;
	mVolumeType = voltype;
	mViewType = viewtype;
        mImageId = image_id;
        mSecondId = second_id;
        mThirdId = third_id;
	

	cameraMoving = false;
	drawInformation = OmPreferences::GetBoolean(OM_PREF_VIEW2D_SHOW_INFO_BOOL);

        //OmVolume &curvol = OmVolume::GetVol(voltype, mImageId);

        if (mVolumeType == CHANNEL) {
                OmChannel &current_channel = OmVolume::GetChannel(mImageId);
                mRootLevel = current_channel.GetRootMipLevel();
		mTilelen = OmVolume::GetChannel(mImageId).GetChunkDimension();
		mVolume = &current_channel;
		mCache = new OmThreadedCachingTile(mViewType, mVolumeType, image_id, &current_channel, NULL);
        } else {
		OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
                mRootLevel = current_seg.GetRootMipLevel();
		mTilelen = OmVolume::GetSegmentation(mImageId).GetChunkDimension();
		mVolume = &current_seg;
        }
}

/*
 *	Constructs View2d widget.
 */
DOmView2d::DOmView2d(ViewType viewtype, ObjectType voltype, OmId image_id, OmId second_id, OmId third_id, QWidget *parent) 
: QGLWidget(parent)
{	
	DOUT("OmView2d::OmView2d -- " << viewtype);
	
	mViewType = viewtype;
	mVolumeType = voltype;
	mImageId = image_id;
	mSecondId = second_id;
	mThirdId = third_id;

	mPainting = false;

	const QGLContext * mycontext = NULL;

	mycontext = this->context();
	
	setFocusPolicy(Qt::ClickFocus);		// necessary for receiving keyboard events
	setMouseTracking(true);				// necessary for mouse-centered zooming
	setAutoFillBackground(false);		// necessary for proper QPainter functionality
	

	// set the cache up
	assert((mVolumeType == CHANNEL) || (mVolumeType == SEGMENTATION));
	
	if (mVolumeType == CHANNEL) {
		OmChannel &current_channel = OmVolume::GetChannel(mImageId);
		
		mCache = new OmThreadedCachingTile(mViewType, mVolumeType, image_id, &current_channel, this->context());
		mCache->SetMaxCacheSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT) * BYTES_PER_MB);
		mCache->SetContinuousUpdate(false);
		
		if(mSecondId && mThirdId) {
			OmChannel &second_channel = OmVolume::GetChannel(mSecondId);
			OmChannel &third_channel = OmVolume::GetChannel(mThirdId);
			DOUT("THERE ARE THREE IDS");
			mCache->setSecondMipVolume(CHANNEL, mSecondId, &second_channel);
		}
		
		mRootLevel = current_channel.GetRootMipLevel();
		
	}
	else {
		OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
		
		mCache = new OmThreadedCachingTile(mViewType, mVolumeType, image_id, &current_seg, mycontext);
		mCache->SetMaxCacheSize(OmPreferences::GetInteger(OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT) * BYTES_PER_MB);
		mCache->SetContinuousUpdate(false);
		
		
		if(mSecondId) {
			OmChannel &second_channel = OmVolume::GetChannel(mSecondId);
			mCache->setSecondMipVolume(CHANNEL, mSecondId, &second_channel);
			
		}
		
		mRootLevel = current_seg.GetRootMipLevel();
	}
	
	//cout << "conversion = " << endl;
	DataCoord test = DataCoord(0,0,0);
	SpaceCoord test2 = DataToSpaceCoord(test);
	//cout << "0 ---> " << test2.z << endl;
	
	test = DataCoord(0,0,1);
	test2 = DataToSpaceCoord(test);
	//cout << "1 ---> " << test2.z << endl;
	
	cameraMoving = false;
	
	drawInformation = OmPreferences::GetBoolean(OM_PREF_VIEW2D_SHOW_INFO_BOOL);
	informationUpdated = false;
	
	mSegmentID = 0;
	mSegmentValue = 0;
	
	sentTexture = false;
	
	depthCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT);
	sidesCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT);
	mipCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT);
	
	delete_dirty = false;
	
	iSentIt = false;
} 



#pragma mark 
#pragma mark GL Event Methods
/////////////////////////////////
///////		 GL Event Methods


// The initializeGL() function is called just once, before paintGL() is called.
void DOmView2d::initializeGL()
{	
	std::cerr << __FUNCTION__ << endl;
	//makeCurrent ();

	// IMPORTANT: To cooperate fully with QPainter, we defer matrix stack operations and attribute initialization until the widget needs to be updated.
	//DOUT("OmView2d::initializeGL	" << "(" << size().width() << ", " << size().height() << ")");
	DOUT("viewtype = " << mViewType);
	
	mTotalViewport.lowerLeftX = 0;
	mTotalViewport.lowerLeftY = 0;
	mTotalViewport.width = size().width();
	mTotalViewport.height = size().height();
	
	// cout << "mtotalviewport = " << mTotalViewport << endl;
	
	mNearClip = -1;
	mFarClip = 1;
	mZoomLevel = 0;

	// cout << "mTotalViewport = " << mTotalViewport << endl;
}

void DOmView2d::resizeEvent (QResizeEvent * event)
{
	std::cerr << "starting " << __FUNCTION__ << this << endl;
}

/*
 *	Window resize event
 */
void DOmView2d::resizeGL(int width, int height)
{ 
	DOUT("OmView2d::resizeGL(" << width << ", " << height << ")");
	std::cerr << "starting " << __FUNCTION__ << this << endl;
	//return;

	
	mTotalViewport.lowerLeftX = 0;
	mTotalViewport.lowerLeftY = 0;
	mTotalViewport.width = width;
	mTotalViewport.height = height;
	
	
	// DOUT("img size = (" << image->size().width() << ", " << image->size().height() << ")");
	
	
	Vector2i translateVector = OmStateManager::Instance()->GetPanDistance(mViewType);
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	
	Vector2i minCoord = Vector2i(mTotalViewport.lowerLeftX + translateVector.x, mTotalViewport.lowerLeftY + translateVector.y);
	SpaceCoord minSpaceCoord = DataToSpaceCoord(DataCoord(minCoord.x, minCoord.y, 0));
	Vector2i maxCoord = Vector2i(mTotalViewport.width + translateVector.x, mTotalViewport.height + translateVector.y);
	SpaceCoord maxSpaceCoord = DataToSpaceCoord(DataCoord(maxCoord.x, maxCoord.y, 0));
	
	if(mViewType == YZ_VIEW) {
		//cout << "minCoord = " << minSpaceCoord << endl;
		//cout << "maxCoord = " << maxSpaceCoord << endl;
		OmStateManager::SetViewSliceMax(mViewType, Vector2f(maxSpaceCoord.y, maxSpaceCoord.x));
		//cout << "mDepth = " << OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	} else {
		//cout << "minCoord = " << minSpaceCoord << endl;
		//cout << "maxCoord = " << maxSpaceCoord << endl;
		OmStateManager::SetViewSliceMax(mViewType, Vector2f(maxSpaceCoord.x, maxSpaceCoord.y));
		//cout << "mDepth = " << OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	}
	
	
	//glEnable(GL_BLEND);							// enable blending for transparency
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);		// set blend function
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::cerr << "ending " << __FUNCTION__ <<  this << endl;
} 

/*
 *	Paint window event.
 */	
void DOmView2d::paintEvent(QPaintEvent *event)
{ 	
	std::cerr << "in DOmView2d::" << __FUNCTION__ << endl;
	//mSA->update();

	//return;

        float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
        Vector2i translateVector = OmStateManager::Instance()->GetPanDistance(mViewType);
	Vector2<int> curzoom = OmStateManager::Instance()->GetZoomLevel();
        mTotalViewport.width = size().width();
        mTotalViewport.height = size().height();

	std::cerr << "width: " << size().width() << " height: " << size().height() << endl;
	std::cerr << "mDepth: " << mDepth << endl;
	std::cerr << "curzoom.x: " << curzoom.x << endl;
	std::cerr << "curzoom.y: " << curzoom.y << endl;
	std::cerr << "translateVector: " << translateVector << endl;


	glMatrixMode (GL_PROJECTION);
	glViewport(mTotalViewport.lowerLeftX, mTotalViewport.lowerLeftY, mTotalViewport.width, mTotalViewport.height);
	glLoadIdentity ();
	
	
	//	DOUT("mTotalViewport = " << mTotalViewport);
	//	DOUT("mViewport = " << mViewport);
	//	DOUT("mViewDepth = " << mViewDepth);
	//	DOUT("mOtherViewport = " << mOtherViewport);
	
	
	glOrtho(	mTotalViewport.lowerLeftX, 			/* left */
			mTotalViewport.width,	/* right */
			mTotalViewport.height,	/* bottom */
			mTotalViewport.lowerLeftY,			/* top */
			mNearClip, mFarClip );
	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Draw();
	
        glPushMatrix();
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        
        glPushMatrix();
	

#if 1
	QPainter painter (this);
	
	// Painting the text at the bottom and around the sides
	if(drawInformation) {
		
		painter.fillRect(QRect(10, mTotalViewport.height - 70, 250, 40), QColor(0, 0, 0, 200));
		painter.setPen(QColor(255, 255, 255, 200));
		painter.setFont(QFont("Helvetica", 12, QFont::Bold, true));
		
		int zlevel = OmStateManager::Instance()->GetZoomLevel().x;
		
		float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
		DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
		int mViewDepth = data_coord.z;
		
		if(mVolumeType == SEGMENTATION)
			painter.drawText(QRect(10, mTotalViewport.height - 65, 250, 30), (Qt::AlignLeft | Qt::TextWordWrap),
							 QString("Mip Level: %1    Depth: %2 \nSegment ID: %3    Value: %4").arg(zlevel)
							 .arg(mViewDepth)
							 .arg(mSegmentID)
							 .arg(mSegmentValue));
		else
			painter.drawText(QRect(10, mTotalViewport.height - 65, 250, 30), (Qt::AlignLeft | Qt::TextWordWrap),
							 QString("Mip Level: %1    Depth: %2").arg(zlevel).arg(mViewDepth));
		
		informationUpdated = false;
	}
	
	QPen the_pen;
	switch(mViewType) {
		case XY_VIEW:
			the_pen.setColor(QColor(Qt::blue));
			break;
		case XZ_VIEW:
			the_pen.setColor(QColor(Qt::green));
			break;
		case YZ_VIEW:
			the_pen.setColor(QColor(Qt::red));
			break;
	}
	
	if(hasFocus())
		the_pen.setWidth(5);
	
	painter.setPen(the_pen);
	
	painter.drawRect(mTotalViewport.lowerLeftX, mTotalViewport.lowerLeftY, mTotalViewport.width - 1, mTotalViewport.height - 1);
	
	// PAINTING TEST LA LA LA LA LA LA
	int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
	int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
	//float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	float scaleFactor = (zoomMipVector.y / 10.0);
	

	painter.end();

	
	if((! cameraMoving) && drawComplete && (! sentTexture)) {
		SendFrameBuffer();
	}
	else if(! drawComplete)
		sentTexture = false;
#endif
	
	glPopMatrix();
	doneCurrent ();
	
	std::cerr << "Done ev:" << event << endl;
} 







#pragma mark 
#pragma mark MouseEvent Methods
/////////////////////////////////
///////		 MouseEvent Methods

void DOmView2d::mousePressEvent(QMouseEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	
	switch(OmStateManager::GetSystemMode()) {
		case NAVIGATION_SYSTEM_MODE: {
			clickPoint.x = event->x();
			clickPoint.y = event->y();
			
			cameraMoving = true;
		}
			break;
			
		case EDIT_SYSTEM_MODE: {
			DOUT("edit mode mouse pressed")
			EditModeMousePressed(event);
		}
			break;
	}
	
	
} 

void DOmView2d::mouseReleaseEvent(QMouseEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	
	switch(OmStateManager::GetSystemMode()) {
		case NAVIGATION_SYSTEM_MODE:
			cameraMoving = false;
			break;
			
		case EDIT_SYSTEM_MODE: {
			DOUT("edit mode mouse released");
			EditModeMouseRelease(event);
		}
			break;
	}
	
	// signal ViewBoxChangeEvent
}


void DOmView2d::mouseMoveEvent(QMouseEvent *event) {
	//return;
	
	switch(OmStateManager::GetSystemMode()) {
		case NAVIGATION_SYSTEM_MODE: {
			
			if(cameraMoving) {
				
				Vector2<float> drag = Vector2<float>((clickPoint.x - event->x()), clickPoint.y - event->y());
				Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
				
				OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<float>(current_pan.x - drag.x, current_pan.y - drag.y));
				
				SpaceCoord panSpaceCoord = DataToSpaceCoord(DataCoord(drag.x, drag.y, 0));
				DOUT("panSpaceCoord = " << panSpaceCoord);
				if(mViewType == YZ_VIEW) {
					Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
					OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x + panSpaceCoord.y, slicemin.y + panSpaceCoord.x));
					
					Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
					OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x + panSpaceCoord.y, slicemax.y + panSpaceCoord.x));
				}
				else {
					
					Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
					OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x + panSpaceCoord.x, slicemin.y + panSpaceCoord.y));
					
					Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
					OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x + panSpaceCoord.x, slicemax.y + panSpaceCoord.y));
				}
				
				clickPoint.x = event->x();
				clickPoint.y = event->y();
				
			}
			
			else if(drawInformation) {
				Vector2f clickPoint = Vector2f(event->x(), event->y());
				
				int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
				int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
				float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
				
				Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
				float scaleFactor = (zoomMipVector.y / 10.0);
				
				Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
				
				DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
				
				int mViewDepth = data_coord.z;
				
				switch(mViewType) {
					case XY_VIEW: {
						DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
						
						
						if (mVolumeType == SEGMENTATION) {
							OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
							mSegmentID = current_seg.GetVoxelSegmentId(dataClickPoint);
							if(mSegmentID)
								mSegmentValue = current_seg.GetValueMappedToSegmentId(mSegmentID);
							else
								mSegmentValue = 0;
							// DOUT("segment ID = " << mSegmentID);
						}
					}
						break;
					case XZ_VIEW: {
						DataCoord dataClickPoint = DataCoord(localClickPoint.x, mViewDepth, localClickPoint.y);
						
						
						if (mVolumeType == SEGMENTATION) {
							OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
							mSegmentID = current_seg.GetVoxelSegmentId(dataClickPoint);
							if(mSegmentID)
								mSegmentValue = current_seg.GetValueMappedToSegmentId(mSegmentID);
							else
								mSegmentValue = 0;
							// DOUT("segment ID = " << mSegmentID);
						}
					}
						break;
					case YZ_VIEW: {
						DataCoord dataClickPoint = DataCoord(mViewDepth, localClickPoint.y, localClickPoint.x);
						
						
						if (mVolumeType == SEGMENTATION) {
							OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
							mSegmentID = current_seg.GetVoxelSegmentId(dataClickPoint);
							if(mSegmentID)
								mSegmentValue = current_seg.GetValueMappedToSegmentId(mSegmentID);
							else
								mSegmentValue = 0;
							// DOUT("segment ID = " << mSegmentID);
						}
					}
				}
				informationUpdated = true;
				update();
			}
			
			
		}
			break;
			
		case EDIT_SYSTEM_MODE: {
			if(!event->buttons()) return;
			EditModeMouseMove(event);
			DOUT("Moving mouse in edit mode");
		}
			break;
	}
}


void DOmView2d::mouseDoubleClickEvent(QMouseEvent *event) { 
	std::cerr << __FUNCTION__ << endl;
	return;
	
	Vector2f clickPoint = Vector2f(event->x(), event->y());
	
	
	int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
	int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
	float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	float scaleFactor = (zoomMipVector.y / 10.0);
	
	Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
	//cout << "localClickPoint double click = " << localClickPoint << endl;
	
	float newDepthX = DataToSpaceCoord(SpaceCoord(0, 0, localClickPoint.x)).z;
	float newDepthY = DataToSpaceCoord(SpaceCoord(0, 0, localClickPoint.y)).z;
	
	if (mVolumeType == SEGMENTATION) {
		switch(OmStateManager::GetSystemMode()) {
			case NAVIGATION_SYSTEM_MODE:
				NavigationModeMouseDoubleClick(event);
				break;
				
			case EDIT_SYSTEM_MODE:
				EditModeMouseDoubleClick(event);
				break;
		}
	}
	
	switch(mViewType) {
		case XY_VIEW: {
			// newDepthX = depth in YZ view
			OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, newDepthX);
			
			// newDepthY = depth in XZ view
			OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, newDepthY);
		}
			break;
		case XZ_VIEW: {
			// newDepthX = depth in YZ view
			OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, newDepthX);
			
			// newDepthY = depth in XY view
			OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, newDepthY);
		}
			break;
		case YZ_VIEW: {
			// newDepthX = depth in XY view
			OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, newDepthX);
			
			// newDepthY = depth in XZ view
			OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, newDepthY);
		}
			break;
	}
	
} 


void DOmView2d::NavigationModeMouseDoubleClick(QMouseEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	return;
	DOUT("Omview2d::NavigationModeMouseDoubleClick");
	//augment if shift pressed
	bool augment_selection = event->modifiers() & Qt::ShiftModifier;
	
	// get ids
	int pick_object_type;
	
	// find segment selected
	
	Vector2f clickPoint = Vector2f(event->x(), event->y());
	
	
	int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
	int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
	float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	float scaleFactor = (zoomMipVector.y / 10.0);
	
	Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
	
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
	
	int mViewDepth = data_coord.z;
	
	DataCoord dataClickPoint;
	switch(mViewType) {
		case XY_VIEW:
			dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
			break;
		case XZ_VIEW:
			dataClickPoint = DataCoord(localClickPoint.x, mViewDepth, localClickPoint.y);
			break;
		case YZ_VIEW:
			dataClickPoint = DataCoord(mViewDepth, localClickPoint.y, localClickPoint.x);
			break;
	}
	OmSegmentation &current_seg = OmVolume::GetSegmentation(mImageId);
	OmId theId = current_seg.GetVoxelSegmentId(dataClickPoint);
	
	// cout << "omSegmentID = " << mSegmentID << endl;
	if(current_seg.IsSegmentValid(theId)) {
		// cout << "segment is valid" << endl;
		//get segment state
		
		//get segment state
		bool new_segment_select_state = !(OmVolume::GetSegmentation( mImageId ).IsSegmentSelected( theId ));
		
		//if not augmenting slection and selecting segment
		if(!augment_selection && new_segment_select_state) {
			//get current selection
			OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mImageId);
			//select new segment, deselect current segments
			OmIds select_segment_ids;
			select_segment_ids.insert(theId);
			(new OmSegmentSelectAction(mImageId, select_segment_ids, r_segmentation.GetSelectedSegmentIds()))->Run();
			
		} else {
			//set state of 
			(new OmSegmentSelectAction(mImageId, theId, new_segment_select_state))->Run();
		}
		
	}
}

#pragma mark 
#pragma mark Edit Mode Methods
/////////////////////////////////
///////		 Edit Mode Methods

void DOmView2d::EditModeMousePressed(QMouseEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	// VoxelEditMouse(event, false);
	// START PAINTING
	return;
	
	if (event->button() == Qt::LeftButton) {
		
		Vector2f clickPoint = Vector2f(event->x(), event->y());

		int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
		int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
		float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
		
		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
		float scaleFactor = (zoomMipVector.y / 10.0);
		
		Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
		
		DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
		int mViewDepth = data_coord.z;
		
		DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
		//cout << "mouse pressed: " << dataClickPoint << endl;
		scribbling = true;
		
		
		// modifiedCoords.clear();

		// okay, dataClickPoint is flat, only valid in XY ortho view.  This is not correct.  Needs to be global voxel coordinate.
		DataCoord globalDataClickPoint;
		switch(mViewType) {
			case XY_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.y, dataClickPoint.z);
				//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
			}
				break;
			case XZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.z, dataClickPoint.y);
			}
				break;
			case YZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.z, dataClickPoint.y, dataClickPoint.x);
			}
				break;
		}
		//cout << "global click point: " << globalDataClickPoint << endl;
		
		//store current selection
		OmId segmentation_id, segment_id;
		bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);
		
		//return if not valid
		if(!valid_edit_selection) return;
		
		//switch on tool mode
		SEGMENT_DATA_TYPE data_value;
		switch(OmStateManager::GetToolMode()) {
			case ADD_VOXEL_MODE:
				//get value associated to segment id
				data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
				break;
				
			case SUBTRACT_VOXEL_MODE:
				data_value = NULL_SEGMENT_DATA;
				break;
				
			default:
				assert(false);
		}
		
		//run action
		(new OmVoxelSetValueAction(segmentation_id, globalDataClickPoint, data_value))->Run();
		
		lastDataPoint = dataClickPoint;
	}
}

void DOmView2d::EditModeMouseRelease(QMouseEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	return;
	// END PAINTING
	if (event->button() == Qt::LeftButton && scribbling) {
		Vector2f clickPoint = Vector2f(event->x(), event->y());
		
		
		int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
		int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
		float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
		
		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
		float scaleFactor = (zoomMipVector.y / 10.0);
		
		Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
		
		DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
		int mViewDepth = data_coord.z;
		
		DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
		//drawLineTo(QPoint(clickPoint.x, clickPoint.y));
		scribbling = false;

		// okay, dataClickPoint is flat, only valid in XY ortho view.  This is not correct.  Needs to be global voxel coordinate.
		DataCoord globalDataClickPoint;
		switch(mViewType) {
			case XY_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.y, dataClickPoint.z);
				//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
			}
				break;
			case XZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.z, dataClickPoint.y);
			}
				break;
			case YZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.z, dataClickPoint.y, dataClickPoint.x);
			}
				break;
		}
		//cout << "global click point: " << globalDataClickPoint << endl;
		
		//store current selection
		OmId segmentation_id, segment_id;
		bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);
		
		//return if not valid
		if(!valid_edit_selection) return;
		
		//switch on tool mode
		SEGMENT_DATA_TYPE data_value;
		switch(OmStateManager::GetToolMode()) {
			case ADD_VOXEL_MODE:
				//get value associated to segment id
				data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
				break;
				
			case SUBTRACT_VOXEL_MODE:
				data_value = NULL_SEGMENT_DATA;
				break;
				
			default:
				assert(false);
		}
		
		//run action
		(new OmVoxelSetValueAction(segmentation_id, globalDataClickPoint, data_value))->Run();
		
		lastDataPoint = dataClickPoint;
	}
}

void DOmView2d::EditModeMouseMove(QMouseEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	// KEEP PAINTING
	DOUT("OmView2d::EditModeMouseMove");
	
	if ((event->buttons() & Qt::LeftButton) && scribbling) {
		Vector2f clickPoint = Vector2f(event->x(), event->y());
		
		
		int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
		int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
		float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
		
		Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
		float scaleFactor = (zoomMipVector.y / 10.0);
		
		Vector2f localClickPoint = Vector2f((clickPoint.x / scaleFactor) - widthTranslate, (clickPoint.y / scaleFactor) - heightTranslate);
		
		DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
		int mViewDepth = data_coord.z;
		DataCoord dataClickPoint = DataCoord(localClickPoint.x, localClickPoint.y, mViewDepth);
		
		// okay, dataClickPoint is flat, only valid in XY ortho view.  This is not correct.  Needs to be global voxel coordinate.
		DataCoord globalDataClickPoint;
		switch(mViewType) {
			case XY_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.y, dataClickPoint.z);
				//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
			}
				break;
			case XZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.z, dataClickPoint.y);
			}
				break;
			case YZ_VIEW: {
				globalDataClickPoint = DataCoord(dataClickPoint.z, dataClickPoint.y, dataClickPoint.x);
			}
				break;
		}
		//cout << "global click point: " << globalDataClickPoint << endl;

		
		//store current selection
		OmId segmentation_id, segment_id;
		bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);
		
		//return if not valid
		if(!valid_edit_selection) return;
		
		//switch on tool mode
		SEGMENT_DATA_TYPE data_value;
		switch(OmStateManager::GetToolMode()) {
			case ADD_VOXEL_MODE:
				//get value associated to segment id
				data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
				break;
				
			case SUBTRACT_VOXEL_MODE:
				data_value = NULL_SEGMENT_DATA;
				break;
				
			default:
				assert(false);
		}
		
		//run action
		(new OmVoxelSetValueAction(segmentation_id, globalDataClickPoint, data_value))->Run();
		
		bresenhamLineDraw(lastDataPoint, dataClickPoint);
		
		lastDataPoint = dataClickPoint;
	}
	update ();	
}

void DOmView2d::EditModeMouseDoubleClick(QMouseEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	return;
	NavigationModeMouseDoubleClick(event);
}

void DOmView2d::EditModeKeyPress(QKeyEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	return;
}


void DOmView2d::bresenhamLineDraw(const DataCoord &first, const DataCoord &second) {
	std::cerr << __FUNCTION__ << endl;
	return;
	
	//store current selection
	OmId segmentation_id, segment_id;
	bool valid_edit_selection = OmSegmentEditor::GetEditSelection(segmentation_id, segment_id);
	
	//return if not valid
	if(!valid_edit_selection) return;
	
	//switch on tool mode
	SEGMENT_DATA_TYPE data_value;
	switch(OmStateManager::GetToolMode()) {
		case ADD_VOXEL_MODE:
			//get value associated to segment id
			data_value = OmVolume::GetSegmentation(segmentation_id).GetValueMappedToSegmentId(segment_id);
			break;
			
		case SUBTRACT_VOXEL_MODE:
			data_value = NULL_SEGMENT_DATA;
			break;
			
		default:
			assert(false);
	}
	
	
	//cout << "first data coord = " << first << endl;
	//cout << "second data coord = " << second << endl;
	//cout << endl;
	
	float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
	int mViewDepth = data_coord.z;
	
	int y1 = second.y;
	int y0 = first.y;
	int x1 = second.x;
	int x0 = first.x;
	
	int dy = y1 - y0;
	int dx = x1 - x0;
	int stepx, stepy;
	
	if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
	if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
	dy <<= 1;                                                  // dy is now 2*dy
	dx <<= 1;                                                  // dx is now 2*dx
	
	DataCoord myDC = DataCoord(x0, y0, mViewDepth);
	
	// myDC is flat, only valid for XY view.  This is not correct.

	DataCoord globalDC;
	switch(mViewType) {
		case XY_VIEW: {
			globalDC = DataCoord(myDC.x, myDC.y, myDC.z);
			//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
		}
			break;
		case XZ_VIEW: {
			globalDC = DataCoord(myDC.x, myDC.z, myDC.y);
		}
			break;
		case YZ_VIEW: {
			globalDC = DataCoord(myDC.z, myDC.y, myDC.x);
		}
			break;
	}
	//cout << "global click point: " << globalDC << endl;
	
	(new OmVoxelSetValueAction(segmentation_id, globalDC, data_value))->Run();
	// cout << "insert: " << DataCoord(x0, y0, 0) << endl;
	
	if (dx > dy) {
		int fraction = dy - (dx >> 1);                         // same as 2*dy - dx
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;                                // same as fraction -= 2*dx
			}
			x0 += stepx;
			fraction += dy;                                    // same as fraction -= 2*dy
			DataCoord myDC = DataCoord(x0, y0, mViewDepth);

			// myDC is flat, only valid for XY view.  This is not correct.
			
			DataCoord globalDC;
			switch(mViewType) {
				case XY_VIEW: {
					globalDC = DataCoord(myDC.x, myDC.y, myDC.z);
					//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
				}
					break;
				case XZ_VIEW: {
					globalDC = DataCoord(myDC.x, myDC.z, myDC.y);
				}
					break;
				case YZ_VIEW: {
					globalDC = DataCoord(myDC.z, myDC.y, myDC.x);
				}
					break;
			}
			//cout << "global click point: " << globalDC << endl;
			
			(new OmVoxelSetValueAction(segmentation_id, globalDC, data_value))->Run();
			// cout << "insert: " << DataCoord(x0, y0, 0) << endl;
		}
	} else {
		int fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			//modifiedCoords.insert(DataCoord(x0, y0, mViewDepth));
			DataCoord myDC = DataCoord(x0, y0, mViewDepth);

			// myDC is flat, only valid for XY view.  This is not correct.
			
			DataCoord globalDC;
			switch(mViewType) {
				case XY_VIEW: {
					globalDC = DataCoord(myDC.x, myDC.y, myDC.z);
					//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
				}
					break;
				case XZ_VIEW: {
					globalDC = DataCoord(myDC.x, myDC.z, myDC.y);
				}
					break;
				case YZ_VIEW: {
					globalDC = DataCoord(myDC.z, myDC.y, myDC.x);
				}
					break;
			}
			//cout << "global click point: " << globalDC << endl;
			
			(new OmVoxelSetValueAction(segmentation_id, globalDC, data_value))->Run();
			// cout << "insert: " << DataCoord(x0, y0, 0) << endl;
		}
	}
	
}



#pragma mark 
#pragma mark KeyEvent Methods
/////////////////////////////////
///////		 KeyEvent Methods

void DOmView2d::wheelEvent ( QWheelEvent * event ) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	DOUT("OmView2d::wheelEvent -- " << mViewType);
	
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;
	
	
	if(numSteps >= 0) {
		// ZOOMING IN
		
		Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
		
		if((current_zoom.y == 10) && (current_zoom.x > 0)) {
			// need to move to previous mip level
			OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x - 1, 6));

                        float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType)*2;
                        DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
                        int mViewDepth = data_coord.z;
                        SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, mViewDepth));
                        OmStateManager::Instance()->SetViewSliceDepth(mViewType, space_coord.z);

		}
		else
			OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x, current_zoom.y + (1 * numSteps)));
	}
	else {
		// ZOOMING OUT
		
		Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
		
		if((current_zoom.y == 6) && (current_zoom.x < mRootLevel)) {
			// need to move to next mip level
			OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x + 1, 10));

                        float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType)/2;
                        DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
                        int mViewDepth = data_coord.z;
                        SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, mViewDepth));
                        OmStateManager::Instance()->SetViewSliceDepth(mViewType, space_coord.z);
		}
		
		else if(current_zoom.y > 1)
			OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x, current_zoom.y - (1 * (-1 * numSteps))));
	}
	
	event->accept();
	update ();	
}


void DOmView2d::keyPressEvent(QKeyEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	DOUT("OmView2d::keyPressEvent -- " << mViewType);
	
	
	switch (event->key()) {
		case Qt::Key_Minus:
		{
			Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
			
			if((current_zoom.y == 6) && (current_zoom.x < mRootLevel)) {
				OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x + 1, 10));
			}
			
			else if(current_zoom.y > 1)
				OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x, current_zoom.y - 1));
		}
			break;
		case Qt::Key_Equal:
		{
			Vector2<int> current_zoom = OmStateManager::Instance()->GetZoomLevel();
			
			if((current_zoom.y == 10) && (current_zoom.x > 0)) {
				OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x - 1, 6));
			}
			else
				OmStateManager::Instance()->SetZoomLevel(Vector2<int>(current_zoom.x, current_zoom.y + 1));
		}
			break;
		case Qt::Key_Right:
		{
			Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
			
			OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x + 5, current_pan.y));
			
			SpaceCoord panSpaceCoord = DataToSpaceCoord(DataCoord(5, 0, 0));
			if(mViewType == YZ_VIEW) {
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x, slicemin.y - panSpaceCoord.x));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x, slicemax.y - panSpaceCoord.x));
			}
			else {
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x - panSpaceCoord.x, slicemin.y));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x - panSpaceCoord.x, slicemax.y));
			}
			
		}
			break;
		case Qt::Key_Left:
		{
			Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
			
			OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x - 5, current_pan.y));
			
			SpaceCoord panSpaceCoord = DataToSpaceCoord(DataCoord(5, 0, 0));
			if(mViewType == YZ_VIEW) {
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x, slicemin.y + panSpaceCoord.x));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x, slicemax.y + panSpaceCoord.x));
			}
			else {
				
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x + panSpaceCoord.x, slicemin.y));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x + panSpaceCoord.x, slicemax.y));
			}
		}
			break;
		case Qt::Key_Up:
		{
			Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
			
			OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x, current_pan.y + 5));
			
			SpaceCoord panSpaceCoord = DataToSpaceCoord(DataCoord(5, 0, 0));
			if(mViewType == YZ_VIEW) {
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x - panSpaceCoord.x, slicemin.y));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x - panSpaceCoord.x, slicemax.y));
			}
			else {
				
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x, slicemin.y - panSpaceCoord.x));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x, slicemax.y - panSpaceCoord.x));
			}
		}
			break;
		case Qt::Key_Down:
		{
			Vector2<int> current_pan = OmStateManager::Instance()->GetPanDistance(mViewType);
			
			OmStateManager::Instance()->SetPanDistance(mViewType, Vector2<int>(current_pan.x, current_pan.y - 5));
			
			SpaceCoord panSpaceCoord = DataToSpaceCoord(DataCoord(5, 0, 0));
			if(mViewType == YZ_VIEW) {
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x + panSpaceCoord.x, slicemin.y));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x + panSpaceCoord.x, slicemax.y));
			}
			else {
				
				Vector2<float> slicemin = OmStateManager::Instance()->GetViewSliceMin(mViewType);
				OmStateManager::Instance()->SetViewSliceMin(mViewType, Vector2<float>(slicemin.x, slicemin.y + panSpaceCoord.x));
				
				Vector2<float> slicemax = OmStateManager::Instance()->GetViewSliceMax(mViewType);
				OmStateManager::Instance()->SetViewSliceMax(mViewType, Vector2<float>(slicemax.x, slicemax.y + panSpaceCoord.x));
			}
		}
			break;
		case Qt::Key_W:
			// MOVE UP THE STACK, CLOSER TO VIEWER
		{
			float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
			DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
			
			int mViewDepth = data_coord.z;
			
			SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, mViewDepth + 1));
			
			OmStateManager::Instance()->SetViewSliceDepth(mViewType, space_coord.z);
		}
			break;
		case Qt::Key_S:
			// MOVE DOWN THE STACK, FARTHER FROM VIEWER
		{
			float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
			DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
			
			int mViewDepth = data_coord.z;
			
			SpaceCoord space_coord = DataToSpaceCoord(DataCoord(0, 0, mViewDepth - 1));
			
			OmStateManager::Instance()->SetViewSliceDepth(mViewType, space_coord.z);
			
		}
			break;
		default:
			break;
			//QGLWidget::keyPressEvent(event);
	}
	update ();
}



#pragma mark 
#pragma mark OmEvent Methods
/////////////////////////////////
///////		 OmEvent Methods

void DOmView2d::PreferenceChangeEvent(OmPreferenceEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	return;
	
	switch(event->GetPreference()) {
			
		case OM_PREF_VIEW2D_SHOW_INFO_BOOL:
		{
			drawInformation = OmPreferences::GetBoolean(OM_PREF_VIEW2D_SHOW_INFO_BOOL);
			//update();
		}
			break;
		case OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT:
		{
			mCache->SetNewAlpha(OmPreferences::GetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT));
			//DOUT("New alpha = " << OmPreferences::GetFloat(OM_PREF_VIEW2D_TRANSPARENT_ALPHA_FLT));
			
			//update();
		}
			break;
		case OM_PREF_VIEW2D_VOLUME_CACHE_SIZE_INT:
		{
		}
			break;
		case OM_PREF_VIEW2D_TILE_CACHE_SIZE_INT:
		{
			//update();
		}
			break;
		case OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT:
		{
			depthCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_DEPTH_CACHE_SIZE_INT);
		}
			break;
		case OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT:
		{
			sidesCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_SIDES_CACHE_SIZE_INT);
		}
			break;
		case OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT:
		{
			mipCache = OmPreferences::GetInteger(OM_PREF_VIEW2D_MIP_CACHE_SIZE_INT);
		}
			break;
		default:
			return;
	}
}

/*
 * Redraw this widget
 */

void DOmView2d::SegmentObjectModificationEvent(OmSegmentEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	return;
	//add/remove segment, change state, change selection
	//valid methods: GetModifiedSegmentIds()
	
	DOUT("OmView2d::SegmentObjectModificationEvent");
	
	if((mVolumeType == SEGMENTATION) && (event->GetModifiedSegmentationId() == mImageId)) {
		modified_Ids = event->GetModifiedSegmentIds();
		delete_dirty = true;
		//update();
	}
	
}

void DOmView2d::SegmentDataModificationEvent(OmSegmentEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	return;
	//voxels of a segment have changed
	//valid methods: GetModifiedSegmentIds()
	
	DOUT("OmView2d::SegmentDataModificationEvent");
	
	if((mVolumeType == SEGMENTATION) && (event->GetModifiedSegmentationId() == mImageId)) {
		modified_Ids = event->GetModifiedSegmentIds();
		delete_dirty = true;
		//update();
	}
}

void DOmView2d::SegmentEditSelectionChangeEvent(OmSegmentEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	return;
	//change segment edit selection
	
	DOUT("OmView2d::SegmentEditSelectionChangeEvent");
	
	if(mVolumeType == SEGMENTATION) {
		// need to update paintbrush, not anything on the screen 
		
		OmId mentationEditId;
		OmId mentEditId;
		
		if(OmSegmentEditor::GetEditSelection(mentationEditId, mentEditId)) {
			
			if(mentationEditId == mImageId) {
				
				const Vector3<float> &color = OmVolume::GetSegmentation(mentationEditId).GetSegment(mentEditId).GetColor();
				
				//DOUT("SETTING EDIT COLOR");
				editColor = qRgba(color.x * 255, color.y * 255, color.z * 255, 255);
			}
		}
	}
}

void DOmView2d::VoxelModificationEvent(OmVoxelEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	return;
	//voxels in a segmentation have been modified
	//valid methods: GetSegmentationId(), GetVoxels()
	
	DOUT("OmView2d::VoxelModificationEvent() ---" << mViewType);
	
	// cout << "voxel modification event" << endl;
	DOUT("event segmentation id = " << event->GetSegmentationId());
	DOUT("iSentIt = " << iSentIt);
	// cout << "event segmentation id: " << event->GetSegmentationId() << endl;
	if((mVolumeType == SEGMENTATION) && (event->GetSegmentationId() == mImageId)) {
		
		set< DataCoord > modVoxels = event->GetVoxels();
		
		// these voxels are not flat, they are correct for ortho views
		
		set<DataCoord>::iterator itr;
		
		for(itr = modVoxels.begin(); itr != modVoxels.end(); itr++) {
			DOUT("data coord = " << *itr);
			modifiedCoords.insert(*itr);
			
		}

		//update();
	}
}


void DOmView2d::SystemModeChangeEvent(OmSystemModeEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	return;
	
	DOUT("OmView2d::SystemModeChangeEvent");
	
	if(mVolumeType == SEGMENTATION) {
		modified_Ids = OmVolume::GetSegmentation(mImageId).GetSelectedSegmentIds();
		delete_dirty = true;
		//update();
	}
	
}




#pragma mark 
#pragma mark Actions
/////////////////////////////////
///////		 Actions


#pragma mark 
#pragma mark ViewEvent Methods
/////////////////////////////////
///////		 ViewEvent Methods

void DOmView2d::ViewBoxChangeEvent(OmViewEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	DOUT("OmView2d::ViewBoxChangeEvent -- " << mViewType);
	update();
}

void DOmView2d::ViewPosChangeEvent(OmViewEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	//return;
	DOUT("OmView2d::ViewPosChangeEvent -- " << mViewType);
	update();
	
}


void DOmView2d::ViewCenterChangeEvent(OmViewEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	return;
	DOUT("OmView2d::ViewCenterChangeEvent");
	
	// update view center
	
	
	// DOUT("mCenter = " << mCenter);
	
	//update();
}

void DOmView2d::ViewRedrawEvent(OmViewEvent *event) {
	std::cerr << __FUNCTION__ << endl;
	return;
	//DOUT("ViewRedrawEvent");
	
	//update();
}




#pragma mark 
#pragma mark Draw Methods
/////////////////////////////////
///////		 Draw Methods

void DOmView2d::Draw()
{
	
	drawComplete = true;
	return;

	DOUT("OmView2d::Draw() -- " << mViewType);
	
	Vector2i translateVector = OmStateManager::Instance()->GetPanDistance(mViewType);
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	
	float zoomFactor = (zoomMipVector.y / 10.0);
	
	//	DOUT("new image size = (" << image->width() << ", " << image->height());
	
	float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	
	
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
	int mDataDepth = data_coord.z;
	
	//cout << "mDepth = " << mDepth << endl;
	//cout << "mDataDepth = " << mDataDepth << endl;
	
	// data coord
	int tileLength;
	switch(mVolumeType) {
		case CHANNEL:
			tileLength = OmVolume::GetChannel(mImageId).GetChunkDimension();
			break;
		case SEGMENTATION:
			tileLength = OmVolume::GetSegmentation(mImageId).GetChunkDimension();
			break;
	}
	
	// take mipchunks starting at (0,0) + translateVector % 128
	// draw at translateVector % 128
	
	
	int xMipChunk;
	int yMipChunk;
	
	int xval;
	int yval;
	
	int Xmodifier;
	if(translateVector.x < 0)
		Xmodifier = -1;
	else
		Xmodifier = 1;
	
	int Ymodifier;
	if(translateVector.y < 0)
		Ymodifier = -1;
	else
		Ymodifier = 1;
	
	
	int tl = tileLength * pow(2, zoomMipVector.x);
	
	if(translateVector.y < 0) {
		yMipChunk =  (abs(translateVector.y) / tl) * tl;
		yval = (Ymodifier * (abs(translateVector.y) % tl));
	}
	else {
		yMipChunk = 0;
		yval = translateVector.y;
	}
	
	for (int y = yval ; y < (mTotalViewport.height * (1.0 / zoomFactor))  ; y = y + tileLength) {
		
		if(translateVector.x < 0) {
			xMipChunk = (abs(translateVector.x) / tl) * tl;
			xval = (Xmodifier * (abs(translateVector.x) % tl));
		}
		else {
			xMipChunk = 0;
			xval = translateVector.x;
		}
		
		for (int x = xval ; x < (mTotalViewport.width * (1.0/zoomFactor))  ; x = x + tileLength) {
			
			//			DOUT("(x, y) = (" << x << ", " << y << ")");
			//			DOUT("mipchunkX = " << xMipChunk);
			//			DOUT("mipchunkY = " << yMipChunk);



                        DataCoord this_data_coord;


			switch(mViewType) {
				case XY_VIEW: {
					this_data_coord = DataCoord(xMipChunk, yMipChunk, mDataDepth);
					//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
				}
					break;
				case XZ_VIEW: {
					this_data_coord = DataCoord(xMipChunk, mDataDepth, yMipChunk);
				}
					break;
				case YZ_VIEW: {
					this_data_coord = DataCoord(mDataDepth, yMipChunk, xMipChunk);
				}
					break;
			}
			

			
			SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);
			
			//cout << "zoom?:" << zoomMipVector.x << "coords: " << this_space_coord << endl;
			//cout << "data:" << this_data_coord << endl;

			OmTileCoord mTileCoord = OmTileCoord(zoomMipVector.x, this_space_coord);	
			
			// PULLING FROM THE CACHE
			DOUT("requested tilecoord = " << mTileCoord);
			shared_ptr<OmTextureID> gotten_id = mCache->GetTextureID(mTileCoord);
			
			
			if(gotten_id) {
				if (OMTILE_NEEDTEXTUREBUILT == gotten_id->flags) {

                        		glEnable (GL_TEXTURE_2D); /* enable texture mapping */
                        		GLuint texture;
                        		glGenTextures( 1, &texture );
                        		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                        		glBindTexture (GL_TEXTURE_2D, texture);
                        		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
                        		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
                        		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

					// HACK ALERT. This needs a big FIXME! MW. Should not use mem_size.
				        glTexImage2D (GL_TEXTURE_2D, 0, GL_LUMINANCE, gotten_id->x, gotten_id->y, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, ((unsigned char*) gotten_id->texture));

					gotten_id->flags = OMTILE_GOOD;
					gotten_id->textureID = texture;

					glDisable (GL_TEXTURE_2D); /* disable texture mapping */

					free (gotten_id->texture);
					gotten_id->texture = NULL;
				}
			}


//			if(gotten_id) {
//				
//				DOUT("requested texture id = " << gotten_id->GetTextureID());
//			}
//			else {
//				
//				DOUT("did not get id!");
//			}
			
			// NOW HAVE TEXTURE ID, need to check if it is dirty
			
			if(gotten_id) {
				
				if(delete_dirty) {
					
					
					if(gotten_id->GetTextureID() != 0) {
						
						//DOUT("texture id = " << gotten_id->GetTextureID());
						
						OmIds::iterator itr;
						if(! modified_Ids.empty()) {
							
							for(itr = modified_Ids.begin(); itr != modified_Ids.end(); itr++) {
								
								
								if(gotten_id->FindId(*itr)) {
									//DOUT("modified id: " << *itr);
									//										cout << "tile coordinate: " << gotten_id->GetCoordinate().Coordinate << endl;
									//										cout << "data tile coordinate: " << SpaceToDataCoord(gotten_id->GetCoordinate().Coordinate) << endl;
									
									DataCoord first_coord = SpaceToDataCoord(gotten_id->GetCoordinate().Coordinate);
									DOUT("tile coord = " << mTileCoord);
									DOUT("first_coord = " << first_coord);
									DOUT("this_data_coord = " << this_data_coord);
									
									DataCoord ortho_coord;
									switch(mViewType) {
										case XY_VIEW: {
											ortho_coord = DataCoord(first_coord.x, first_coord.y, first_coord.z);
											//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
										}
											break;
										case XZ_VIEW: {
											ortho_coord = DataCoord(first_coord.x, first_coord.z, first_coord.y);
										}
											break;
										case YZ_VIEW: {
											ortho_coord = DataCoord(first_coord.z, first_coord.y, first_coord.x);
										}
											break;
									}
									DOUT("ortho_coord = " << ortho_coord);
									
									mCache->subFullImageTex(gotten_id, ortho_coord, tileLength);
									break;
								}
							}
						}
					}
				}
			}
			
			if(gotten_id) {
				
				if(gotten_id->GetTextureID() != 0) {
					//	DOUT("texture is valid! : ");
					
					// REPLACE VOXELS
					
					
					if(! modifiedCoords.empty()) {
						DOUT("modified coords are not empty");
						
						set<DataCoord>::iterator itr;
						set<DataCoord> newCoord;
						
						set<DataCoord> continueCoords;
						
						for(itr = modifiedCoords.begin(); itr != modifiedCoords.end(); itr++) {
							DataCoord dataClickPoint = *itr;
							
							DOUT("dataclickpoint = " << *itr);
							
							// data click point is global, good for all orthogonal views.
							// need to convert data click point into flat view for use in determining whether these voxels are actually being displayed
							
							DataCoord flatClickPoint;
							switch(mViewType) {
								case XY_VIEW: {
									flatClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.y, dataClickPoint.z);
									//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
								}
									break;
								case XZ_VIEW: {
									// ortho coord: (x, z, y) need (x, y, z)
									flatClickPoint = DataCoord(dataClickPoint.x, dataClickPoint.z, dataClickPoint.y);
								}
									break;
								case YZ_VIEW: {
									// ortho coord: (z, y, x) need (x, y, z)
									flatClickPoint = DataCoord(dataClickPoint.z, dataClickPoint.y, dataClickPoint.x);
								}
									break;
							}
							//cout << "flat click point: " << flatClickPoint << endl;
							
							DataCoord yzFlatDataClickPoint;
							if(mViewType == YZ_VIEW) {
								yzFlatDataClickPoint = DataCoord(flatClickPoint.y, flatClickPoint.x, flatClickPoint.z);
							}
							else
								yzFlatDataClickPoint = flatClickPoint;
							
							// DON'T ignore weirdness of yz view for now
							
							int xData = flatClickPoint.x;
							int yData = flatClickPoint.y;
							
							//cout << "yzDataClickPoint = " << yzDataClickPoint << endl;
							
							//DOUT("data click point = " << dataClickPoint);
							//							DOUT("x mip chunk = " << xMipChunk);
							//							DOUT("y mip chunk = " << yMipChunk);
							
							if(((xData >= xMipChunk) && (xData < xMipChunk + tileLength)) &&
							   ((yData >= yMipChunk) && (yData < yMipChunk + tileLength)) &&
							   (flatClickPoint.x == mDataDepth)){
								// this is the correct texture for draw
								//								DOUT("this is the correct texture for draw");
								
								// sending out flat data which was used to calculate position
								cout << "sending: " << yzFlatDataClickPoint << " ------ view = " << mViewType << endl;
								newCoord.insert(yzFlatDataClickPoint);
								mCache->subImageTex(gotten_id, 1, newCoord, editColor, tileLength);
								newCoord.clear();
								
								iSentIt = true;
								
							}
							else
								continueCoords.insert(dataClickPoint);
						}	
						modifiedCoords = continueCoords;
					}	
				}
			}
			iSentIt = false;
			
			
			glEnable (GL_TEXTURE_2D); /* enable texture mapping */			
			
			if(mViewType == YZ_VIEW) {
				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();
				glTranslatef(0.5,0.5,0.0);
				glRotatef(-90,0.0,0.0,1.0);
				glTranslatef(-0.5,-0.5,0.0);
				glMatrixMode(GL_MODELVIEW); 
			}
			
			if(gotten_id) {
				
				if(gotten_id->GetTextureID() != 0) {
					DOUT("texture is valid! : ");
					
					
					
					glBindTexture (GL_TEXTURE_2D, gotten_id->GetTextureID());
					glBegin (GL_QUADS);
					
					if(mViewType == XY_VIEW) {
						glTexCoord2f (0.0f,0.0f); /* lower left corner of image */
						//glVertex2f(i.key().get<1>() * tileLength, i.key().get<2>() * tileLength);
						glVertex2f(x * zoomFactor, y * zoomFactor);
						
						glTexCoord2f (1.0f, 0.0f); /* lower right corner of image */
						glVertex2f((x + tileLength) * zoomFactor, y * zoomFactor);
						
						glTexCoord2f (1.0f, 1.0f); /* upper right corner of image */
						glVertex2f((x + tileLength) * zoomFactor, (y + tileLength) * zoomFactor);
						
						glTexCoord2f (0.0f, 1.0f); /* upper left corner of image */
						glVertex2f(x * zoomFactor, (y + tileLength) * zoomFactor);
						glEnd ();
					}
					
					else if(mViewType == XZ_VIEW) {
						
						glTexCoord2f (0.0f,0.0f); /* lower left corner of image */
						glVertex2f(x * zoomFactor, y * zoomFactor);
						
						glTexCoord2f (1.0f, 0.0f); /* lower right corner of image */
						glVertex2f((x + tileLength) * zoomFactor, y * zoomFactor);
						
						glTexCoord2f (1.0f, 1.0f); /* upper right corner of image */
						glVertex2f((x + tileLength) * zoomFactor, (y + tileLength) * zoomFactor);
						
						glTexCoord2f (0.0f, 1.0f); /* upper left corner of image */
						glVertex2f(x * zoomFactor, (y + tileLength) * zoomFactor);
						glEnd ();
					}
					else if(mViewType == YZ_VIEW) {
						
						glTexCoord2f (0.0f,0.0f); /* lower left corner of image */
						glVertex2f((x + tileLength) * zoomFactor, y * zoomFactor);
						
						glTexCoord2f (1.0f, 0.0f); /* lower right corner of image */
						glVertex2f(x * zoomFactor, y * zoomFactor);
						
						glTexCoord2f (1.0f, 1.0f); /* upper right corner of image */
						glVertex2f(x * zoomFactor, (y + tileLength) * zoomFactor);
						
						glTexCoord2f (0.0f, 1.0f); /* upper left corner of image */
						glVertex2f((x + tileLength) * zoomFactor, (y + tileLength) * zoomFactor);
						glEnd ();
					}
					
					xMipChunk = xMipChunk + tl;
				}
			}
			else {
				// draw gray texture
				glColor3f (0.2F, 0.2F, 0.2F);
				glBegin (GL_QUADS);
				
				if(mViewType == XY_VIEW) {
					glVertex2f(x * zoomFactor, y * zoomFactor);
					glVertex2f((x + tileLength) * zoomFactor, y * zoomFactor);
					glVertex2f((x + tileLength) * zoomFactor, (y + tileLength) * zoomFactor);
					glVertex2f(x * zoomFactor, (y + tileLength) * zoomFactor);
					glEnd ();
				}
				
				else if(mViewType == XZ_VIEW) {
					glVertex2f(x * zoomFactor, y * zoomFactor);
					glVertex2f((x + tileLength) * zoomFactor, y * zoomFactor);
					glVertex2f((x + tileLength) * zoomFactor, (y + tileLength) * zoomFactor);
					glVertex2f(x * zoomFactor, (y + tileLength) * zoomFactor);
					glEnd ();
				}
				else if(mViewType == YZ_VIEW) {
					glVertex2f((x + tileLength) * zoomFactor, y * zoomFactor);
					glVertex2f(x * zoomFactor, y * zoomFactor);
					glVertex2f(x * zoomFactor, (y + tileLength) * zoomFactor);
					glVertex2f((x + tileLength) * zoomFactor, (y + tileLength) * zoomFactor);
					glEnd ();
				}
				// DOUT("DRAW NOT COMPLETE");
				drawComplete = false;
				glColor3f(1.0F, 1.0F, 1.0F);
			}
			
		}
		// if (gotten_id->GetTextureID() != 0)
		yMipChunk = yMipChunk + tl;
	}
	
	glDisable (GL_TEXTURE_2D); /* disable texture mapping */ 
	
	DrawCursors();
	
}


void DOmView2d::SendFrameBuffer()
{
	//bwarne: temp fix...
	return;
	
#if 0
	sentTexture = true;
	DOUT("DRAW IS COMPLETE DRAW IS COMPLETE DRAW IS COMPLETE DRAW IS COMPLETE");
	
	glEnable (GL_TEXTURE_2D); /* enable texture mapping */		
	
	cout << "not got tex image yet" << endl;
	
	OmStateManager::SetSliceState(SLICE_XY_PLANE, true);
	OmStateManager::SetSliceState(SLICE_XZ_PLANE, true);
	OmStateManager::SetSliceState(SLICE_YZ_PLANE, true);
	
	//QImage *img = new QImage(grabFrameBuffer(true));
	//unsigned char *imgData = img->bits();
	
	OmStateManager::SetViewSliceDataFormat(4, 1);
	
	switch(mViewType) {
		case XY_VIEW: {
			DOUT("setting view slice");
			OmStateManager::SetSliceState(SLICE_XY_PLANE, true);
			OmStateManager::SetViewSlice(SLICE_XY_PLANE, Vector3<int>(size().width(),
																	  size().height(),
																	  1),
										 imgData);
			
		}
			break;
		case XZ_VIEW: {
			OmStateManager::SetSliceState(SLICE_XZ_PLANE, true);
			OmStateManager::SetViewSlice(SLICE_XZ_PLANE, Vector3<int>(size().width(),
																	  size().height(),
																	  1),
										 imgData);
		}
			break;
		case YZ_VIEW: {
			OmStateManager::SetSliceState(SLICE_YZ_PLANE, true);
			OmStateManager::SetViewSlice(SLICE_YZ_PLANE, Vector3<int>(size().width(),
																	  size().height(),
																	  1),
										 imgData);
		}
			break;
	}
	
	glDisable (GL_TEXTURE_2D); /* disable texture mapping */ 
	
	//	img->save( "test.bmp", "BMP");
	
	delete img;
	delete imgData;
	
	sentTexture = true;
	
#endif

}

void DOmView2d::InitializeCache()
{
	DOUT("OmView2d::InitializeCache() -- " << mViewType);
	// cout << "INITIALIZING CACHE" << endl;
	Vector2i translateVector = OmStateManager::Instance()->GetPanDistance(mViewType);
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	
	float zoomFactor = (zoomMipVector.y / 10.0);
	
	float mDepth = OmStateManager::Instance()->GetViewSliceDepth(mViewType);
	
	DataCoord data_coord = SpaceToDataCoord(SpaceCoord(0, 0, mDepth));
	int mDataDepth = data_coord.z;
	
	glEnable (GL_TEXTURE_2D); /* enable texture mapping */
	
	// data coord
	int tileLength;
	switch(mVolumeType) {
		case CHANNEL:
			tileLength = OmVolume::GetChannel(mImageId).GetChunkDimension();
			break;
		case SEGMENTATION:
			tileLength = OmVolume::GetSegmentation(mImageId).GetChunkDimension();
			break;
	}
	
	
	int xMipChunk;
	int yMipChunk;
	
	int xval;
	int yval;
	
	int Xmodifier;
	if(translateVector.x < 0)
		Xmodifier = -1;
	else
		Xmodifier = 1;
	
	int Ymodifier;
	if(translateVector.y < 0)
		Ymodifier = -1;
	else
		Ymodifier = 1;
	
	
	// SECOND MIP LEVEL
	
	int tl = tileLength * pow(2, mipCache);
	// (zoomMipVector.x + 1);
	
	if(translateVector.y < 0) {
		yMipChunk =  (abs(translateVector.y) / tl) * tl;
		yval = (Ymodifier * (abs(translateVector.y) % tl));
	}
	else {
		yMipChunk = 0;
		yval = translateVector.y;
	}
	
	for (int y = 0 ; y < (mTotalViewport.height + 5*tileLength)  ; y = y + tileLength) {
		
		if(translateVector.x < 0) {
			xMipChunk = (abs(translateVector.x) / tl) * tl;
			xval = (Xmodifier * (abs(translateVector.x) % tl));
		}
		else {
			xMipChunk = 0;
			xval = translateVector.x;
		}
		
		for (int x = 0 ; x < (mTotalViewport.width + 5*tileLength)  ; x = x + tileLength) {
			//			cout << "x = " << x << endl;
			//				cout << "d = " << d << endl;
			
			//			DOUT("(x, y) = (" << x << ", " << y << ")");
			//			DOUT("mipchunkX = " << xMipChunk);
			//			DOUT("mipchunkY = " << yMipChunk);
			// cout << "d = " << d << endl;
			DataCoord this_data_coord;
			switch(mViewType) {
				case XY_VIEW: {
					this_data_coord = DataCoord(xMipChunk, yMipChunk, mDataDepth);
					//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
				}
					break;
				case XZ_VIEW: {
					this_data_coord = DataCoord(xMipChunk, mDataDepth, yMipChunk);
				}
					break;
				case YZ_VIEW: {
					this_data_coord = DataCoord(mDataDepth, yMipChunk, xMipChunk);
				}
					break;
			}
			
			
			SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);
			
			OmTileCoord mTileCoord = OmTileCoord(1, this_space_coord);	
			
			shared_ptr<OmTextureID> gotten_id = mCache->GetTextureID(mTileCoord);
			
			if(gotten_id) {
				
				
				if(delete_dirty) {
					
					if(gotten_id->GetTextureID() != 0) {
						
						OmIds::iterator itr;
						if(! modified_Ids.empty()) {
							
							for(itr = modified_Ids.begin(); itr != modified_Ids.end(); itr++) {
								
								//DOUT("modified id: " << *itr);
								if(gotten_id->FindId(*itr)) {
									//DOUT("modified id: " << *itr);
									//										cout << "tile coordinate: " << gotten_id->GetCoordinate().Coordinate << endl;
									//										cout << "data tile coordinate: " << SpaceToDataCoord(gotten_id->GetCoordinate().Coordinate) << endl;
									
									DataCoord first_coord = SpaceToDataCoord(gotten_id->GetCoordinate().Coordinate);
									
									mCache->subFullImageTex(gotten_id, first_coord, tileLength);
									
									break;
								}
							}
						}
					}
				}
			}
			
			xMipChunk = xMipChunk + tl;
		}
		yMipChunk = yMipChunk + tl;
	}
	
	
	
	
	tl = tileLength * pow(2, zoomMipVector.x);
	
	if(translateVector.y < 0) {
		yMipChunk =  (abs(translateVector.y) / tl) * tl;
		yval = (Ymodifier * (abs(translateVector.y) % tl));
	}
	else {
		yMipChunk = 0;
		yval = translateVector.y;
	}
	
	for (int y = 0 ; y < (mTotalViewport.height + sidesCache*tileLength)  ; y = y + tileLength) {
		
		if(translateVector.x < 0) {
			xMipChunk = (abs(translateVector.x) / tl) * tl;
			xval = (Xmodifier * (abs(translateVector.x) % tl));
		}
		else {
			xMipChunk = 0;
			xval = translateVector.x;
		}
		
		for (int x = 0 ; x < (mTotalViewport.width + sidesCache*tileLength)  ; x = x + tileLength) {
			//			cout << "x = " << x << endl;
			for(float d = mDataDepth + 1 ; d < mDataDepth + depthCache ; d = d++) {
				//				cout << "d = " << d << endl;
				
				//			DOUT("(x, y) = (" << x << ", " << y << ")");
				//			DOUT("mipchunkX = " << xMipChunk);
				//			DOUT("mipchunkY = " << yMipChunk);
				// cout << "d = " << d << endl;
				DataCoord this_data_coord;
				switch(mViewType) {
					case XY_VIEW: {
						this_data_coord = DataCoord(xMipChunk, yMipChunk, d);
						//					cout << "Pulling --> " << xMipChunk << " " << yMipChunk << " " << mDataDepth << endl;
					}
						break;
					case XZ_VIEW: {
						this_data_coord = DataCoord(xMipChunk, d, yMipChunk);
					}
						break;
					case YZ_VIEW: {
						this_data_coord = DataCoord(d, yMipChunk, xMipChunk);
					}
						break;
				}
				
				
				SpaceCoord this_space_coord = DataToSpaceCoord(this_data_coord);
				
				OmTileCoord mTileCoord = OmTileCoord(zoomMipVector.x, this_space_coord);	
				
				shared_ptr<OmTextureID> gotten_id = mCache->GetTextureID(mTileCoord);
				
				if(gotten_id) {
					
					if(delete_dirty) {
						
						if(gotten_id->GetTextureID() != 0) {
							
							OmIds::iterator itr;
							if(! modified_Ids.empty()) {
								
								for(itr = modified_Ids.begin(); itr != modified_Ids.end(); itr++) {
									
									//DOUT("modified id: " << *itr);
									if(gotten_id->FindId(*itr)) {
										//DOUT("modified id: " << *itr);
										//											cout << "tile coordinate: " << gotten_id->GetCoordinate().Coordinate << endl;
										//											cout << "data tile coordinate: " << SpaceToDataCoord(gotten_id->GetCoordinate().Coordinate) << endl;
										
										
										//DOUT("prior release");
										gotten_id.reset();
										mCache->Remove(mTileCoord);
										//DOUT("after release");
										// DELETED THIS TEXTURE ID AND FORCED RELOAD
										shared_ptr<OmTextureID> gotten_id = mCache->GetTextureID(mTileCoord);
										
										break;
									}
								}
							}
						}
					}
				}
				
			}
			xMipChunk = xMipChunk + tl;
		}
		yMipChunk = yMipChunk + tl;
	}
	
	
	glDisable (GL_TEXTURE_2D); /* disable texture mapping */ 
	
	delete_dirty = false;
}


/*
 *	Draw all enabled widgets.
 */
void DOmView2d::DrawCursors() {
	
	int widthTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).x;
	int heightTranslate = OmStateManager::Instance()->GetPanDistance(mViewType).y;
	
	Vector2i zoomMipVector = OmStateManager::Instance()->GetZoomLevel();
	float scaleFactor = (zoomMipVector.y / 10.0);
	
	// scaleFactor = 1.0;
	
	// convert mCenter to data coordinates
	SpaceCoord YZslice = SpaceCoord(0, 0,
									OmStateManager::Instance()->GetViewSliceDepth(YZ_VIEW));
	
	SpaceCoord XZslice = SpaceCoord(0, 0,
									OmStateManager::Instance()->GetViewSliceDepth(XZ_VIEW));
	
	SpaceCoord XYslice = SpaceCoord(0, 0,
									OmStateManager::Instance()->GetViewSliceDepth(XY_VIEW));
	
	
	// cout << "SLICE DEPTHS: " << mCenter  << endl;
	
	
	int YZslice_depth = SpaceToDataCoord(YZslice).z;
	int XZslice_depth = SpaceToDataCoord(XZslice).z;
	int XYslice_depth = SpaceToDataCoord(XYslice).z;
	
	//mView3dWidgetManager.CallEnabled( &OmView3dWidget::Draw )
	
	switch(mViewType) {
		case XY_VIEW: {
			//			DOUT("Y DEPTH = " << cur_data_center.y);
			//			DOUT("X DEPTH = " << cur_data_center.x);
			glColor3f (0.2F, 0.9F, 0.2F);
			glBegin(GL_LINES);
			glVertex2i(mTotalViewport.x, (XZslice_depth + heightTranslate) * scaleFactor); // origin of the line
			glVertex2i(mTotalViewport.x + mTotalViewport.width, (XZslice_depth + heightTranslate) * scaleFactor); // ending point of the line
			
			glEnd( );
			
			glColor3f (0.9F, 0.2F, 0.2F);
			glBegin(GL_LINES);
			glVertex2i((YZslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y); // origin of the line
			glVertex2i((YZslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y + mTotalViewport.height); // ending point of the line
			glEnd( );
			
			//			cout << "XZ line = " << (XZslice_depth + heightTranslate) * scaleFactor << endl;
			//			cout << "YZ line = " << (YZslice_depth + widthTranslate) * scaleFactor << endl;
		}
			break;
		case XZ_VIEW: {
			//			DOUT("Z DEPTH = " << cur_data_center.z);
			//			DOUT("X DEPTH = " << cur_data_center.x);
			glColor3f (0.2F, 0.2F, 0.9F);
			glBegin(GL_LINES);
			glVertex2i(mTotalViewport.x, (XYslice_depth + heightTranslate) * scaleFactor); // origin of the line
			glVertex2i(mTotalViewport.x + mTotalViewport.width, (XYslice_depth + heightTranslate) * scaleFactor); // ending point of the line
			glEnd( );
			
			glColor3f (0.9F, 0.2F, 0.2F);
			glBegin(GL_LINES);
			glVertex2i((YZslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y); // origin of the line
			glVertex2i((YZslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y + mTotalViewport.height); // ending point of the line
			glEnd( );
		}
			break;
		case YZ_VIEW: {
			//			DOUT("Z DEPTH = " << cur_data_center);
			//			DOUT("Y DEPTH = " << cur_data_center.y);
			glColor3f (0.2F, 0.2F, 0.9F);
			glBegin(GL_LINES);
			glVertex2i((XYslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y); // origin of the line
			glVertex2i((XYslice_depth + widthTranslate) * scaleFactor, mTotalViewport.y + mTotalViewport.height); // ending point of the line
			glEnd( );
			
			glColor3f (0.2F, 0.9F, 0.2F);
			glBegin(GL_LINES);
			glVertex2i(mTotalViewport.x, (XZslice_depth + heightTranslate) * scaleFactor); // origin of the line
			glVertex2i(mTotalViewport.x + mTotalViewport.width, (XZslice_depth + heightTranslate) * scaleFactor); // ending point of the line
			glEnd( );
		}
			break;
	}
	glColor3f(1.0F, 1.0F, 1.0F);
}



DataBbox DOmView2d::SpaceToDataBbox(const SpaceBbox &spacebox) {
	DataBbox new_data_box;
	
	switch(mVolumeType) {
		case CHANNEL:
			new_data_box = OmVolume::NormToDataBbox(OmVolume::SpaceToNormBbox(spacebox));
			break;
		case SEGMENTATION:
			new_data_box = OmVolume::NormToDataBbox(OmVolume::SpaceToNormBbox(spacebox));
			break;
			
	}
	
	return new_data_box;
}

SpaceBbox DOmView2d::DataToSpaceBbox(const DataBbox &databox) {
	SpaceBbox new_space_box;
	
	switch(mVolumeType) {
		case CHANNEL:
			new_space_box = OmVolume::NormToSpaceBbox(OmVolume::DataToNormBbox(databox));
			break;
		case SEGMENTATION:
			new_space_box = OmVolume::NormToSpaceBbox(OmVolume::DataToNormBbox(databox));
			break;		
	}
	
	return new_space_box;
}


DataCoord DOmView2d::SpaceToDataCoord(const SpaceCoord &spacec) {
	
	DataCoord new_data_center;
	
	switch(mVolumeType) {
		case CHANNEL:
			new_data_center = OmVolume::NormToDataCoord(OmVolume::SpaceToNormCoord(spacec));
			break;
		case SEGMENTATION:
			new_data_center = OmVolume::NormToDataCoord(OmVolume::SpaceToNormCoord(spacec));
			break;
			
	}
	
	return new_data_center;
}


SpaceCoord DOmView2d::DataToSpaceCoord(const DataCoord &datac) {
	SpaceCoord new_space_center;
	
	switch(mVolumeType) {
		case CHANNEL:
			new_space_center = OmVolume::NormToSpaceCoord(OmVolume::DataToNormCoord(datac));
			break;
		case SEGMENTATION:
			new_space_center = OmVolume::NormToSpaceCoord(OmVolume::DataToNormCoord(datac));
			break;
			
	}
	return new_space_center;
}





