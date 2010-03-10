
#include "omVoxelSetAction.h"

#include "segment/omSegmentEditor.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

#include "system/omStateManager.h"
#include "system/omEventManager.h"
#include "system/events/omVoxelEvent.h"

#define DEBUG 0

/////////////////////////////////
///////
///////          OmVoxelSetAction Class
///////

/*
 *	Unspecified value uses the Edit Tool and Edit Selection to determine
 *	if a voxel should be added or removed and to what value.
 *	Only call in ADD_VOXEL_MODE or SUBTRACT_VOXEL_MODE.
 */

OmVoxelSetAction::OmVoxelSetAction(DataCoord & voxel)
 : mVoxel(voxel)
{

	//store current selection
	bool valid_edit_selection = OmSegmentEditor::GetEditSelection(mSegmentationId, mSegmentId);

	//if edit selection not valid
	if (!valid_edit_selection) {
		OmAction::SetValid(false);
		return;
	}
	//get segmentation
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//get current voxel value
	mPrevDataValue = r_segmentation.GetVoxelValue(mVoxel);

	//switch on tool mode
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		mDataValue = r_segmentation.GetValueMappedToSegmentId(mSegmentId);
		break;

	case SUBTRACT_VOXEL_MODE:
		mDataValue = NULL_SEGMENT_DATA;
		break;

	default:
		assert(false);
	}

}

/////////////////////////////////
///////          Action Methods

void
 OmVoxelSetAction::Action()
{
	//set voxel
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//set voxel to stored segment id
	r_segmentation.SetVoxelValue(mVoxel, mDataValue);

	//post modification
	OmEventManager::PostEvent(new OmVoxelEvent(OmVoxelEvent::VOXEL_MODIFICATION, mSegmentationId, mVoxel));
}

void OmVoxelSetAction::UndoAction()
{
	//set voxel
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//set voxel to stored segment id
	r_segmentation.SetVoxelValue(mVoxel, mPrevDataValue);

	//post modification
	OmEventManager::PostEvent(new OmVoxelEvent(OmVoxelEvent::VOXEL_MODIFICATION, mSegmentationId, mVoxel));
}

string OmVoxelSetAction::Description()
{

	if (NULL_SEGMENT_DATA == mDataValue) {
		return "Remove Voxel";
	} else {
		//get segmentation
		OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mSegmentationId);
		OmSegment & r_segment = r_segmentation.GetSegment(mSegmentId);

		return string("Set Voxel To ") + r_segment.GetName();
	}
}
