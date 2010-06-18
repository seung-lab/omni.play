#ifndef OM_VIEW3D_UI_H
#define OM_VIEW3D_UI_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 4/21/09
 */


#include "gui/omSegmentContextMenu.h"
#include "system/viewGroup/omViewGroupState.h"

#include <QtGui> 

class OmView3d;

class OmView3dUi {

public:
	OmView3dUi(OmView3d * view3d, OmViewGroupState * );
	
	//ui events
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
	void NavigationModeKeyPress(QKeyEvent *event);
	
	//edit mode
	void EditModeMousePressed(QMouseEvent *event);
	void EditModeMouseRelease(QMouseEvent *event);
	void EditModeMouseMove(QMouseEvent *event);
	void EditModeMouseDoubleClick(QMouseEvent *event);
	void EditModeMouseWheel(QWheelEvent *event);
	void EditModeKeyPress(QKeyEvent *event);

	//dend mode
	void DendModeMouseReleased(QMouseEvent *event);
	
	
	//camera movement
	void CameraMovementMouseStart(QMouseEvent *event);
	void CameraMovementMouseEnd(QMouseEvent *event);
	void CameraMovementMouseUpdate(QMouseEvent *event);
	void CameraMovementMouseWheel(QWheelEvent *event);
	
	//segment picking
	bool PickSegmentMouse(QMouseEvent *event, bool drag, OmId &segmentationId, 
			      OmId &segmentId, int *pickName = NULL);      
	
	//voxel picking
	bool PickVoxel(QKeyEvent *keyEvent, QMouseEvent *mouseEvent, bool drag, DataCoord &voxel);
	bool PickVoxelMouse(QMouseEvent *mouseEvent, bool drag, DataCoord &voxel);
	bool PickVoxelCameraFocus(DataCoord &voxel);
	
	
	//segment actions
	void SegmentSelectToggleMouse(QMouseEvent *event, bool drag);
	
	
	//voxel actions
	void VoxelEditMouse(QMouseEvent *mouseEvent, bool drag);
	
	void VoxelSelectToggleMouse(QMouseEvent *mouseEvent, bool drag);
	void VoxelSelectToggleKey(QKeyEvent *keyEvent, bool augment);
	
	void VoxelSetMouse(QMouseEvent *mouseEvent, bool drag);
	void VoxelSetKey(QKeyEvent *, bool augment);
	
	
	//segment context menu
	void ShowSegmentContextMenu(QMouseEvent *event);
	
	
private:
	OmView3d * const mpView3d;
	OmViewGroupState * mViewGroupState;

	OmSegmentContextMenu mSegmentContextMenu;
	
	bool mPrevSelectionState;
	DataCoord mPrevMouseSelectVoxel;
	OmId mPrevSegmentationId, mPrevSegmentId;

	void crosshair(QMouseEvent * event);
	void CenterAxisOfRotation(QMouseEvent * event);
	OmId PickVoxelMouseCrosshair(QMouseEvent * event, DataCoord & rVoxel);
	bool mCPressed;
};



#endif
