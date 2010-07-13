#include "datalayer/fs/omActionLoggerFS.h"
#include "omVoxelSetAction.h"
#include "project/omProject.h"
#include "segment/actions/omSegmentEditor.h"
#include "segment/omSegmentCache.h"
#include "system/events/omVoxelEvent.h"
#include "system/omEventManager.h"
#include "system/omStateManager.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "utility/dataWrappers.h"

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
	SegmentDataWrapper sdw = OmSegmentEditor::GetEditSelection();
	mSegmentationId = sdw.getSegmentationID();
	mSegmentId = sdw.getID();

	//if edit selection not valid
	if (!sdw.isValid()) {
		OmAction::SetValid(false);
		return;
	}
	//get segmentation
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);

	//get current voxel value
	mPrevDataValue = r_segmentation.GetVoxelValue(mVoxel);

	//switch on tool mode
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		mDataValue = mSegmentId;
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
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);

	//set voxel to stored segment id
	r_segmentation.SetVoxelValue(mVoxel, mDataValue);

	//post modification
	OmEventManager::PostEvent(new OmVoxelEvent(OmVoxelEvent::VOXEL_MODIFICATION, mSegmentationId, mVoxel));
}

void OmVoxelSetAction::UndoAction()
{
	//set voxel
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);

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
		OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);
		OmSegment * r_segment = r_segmentation.GetSegmentCache()->GetSegment(mSegmentId);

		return string("Set Voxel To ") + r_segment->GetName().toStdString();
	}
}

void OmVoxelSetAction::save(const string & comment)
{
	OmActionLoggerFS::save(this, comment);
}
