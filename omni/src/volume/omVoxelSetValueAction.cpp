#include "datalayer/fs/omActionLoggerFS.h"
#include "project/omProject.h"
#include "omVoxelSetValueAction.h"
#include "segment/actions/omSegmentEditor.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "system/omStateManager.h"
#include "system/omEventManager.h"
#include "common/omCommon.h"

/////////////////////////////////
///////
///////          OmVoxelSetValueAction Class
///////

OmVoxelSetValueAction::OmVoxelSetValueAction(OmId segmentationId, DataCoord & rVoxel, OmSegID value)
{
	//store segmentation id
	mSegmentationId = segmentationId;

	//store new value
	mNewValue = value;

	//store old value of voxel
	//mOldVoxelValues[rVoxel] = r_segmentation.GetVoxelValue(rVoxel);
	mOldVoxelValues[rVoxel] = mNewValue;

	mUndoable = false;
}

OmVoxelSetValueAction::OmVoxelSetValueAction(OmId segmentationId, set < DataCoord > &rVoxels, OmSegID value)
{
	//store segmentation id
	mSegmentationId = segmentationId;

	//store new value
	mNewValue = value;

	//store old values of voxels
	set < DataCoord >::iterator itr;
	for (itr = rVoxels.begin(); itr != rVoxels.end(); itr++) {
		//mOldVoxelValues[*itr] = r_segmentation.GetVoxelValue(*itr);
		mOldVoxelValues[*itr] = mNewValue;
	}

	mUndoable = false;
}

/////////////////////////////////
///////          Action Methods

void OmVoxelSetValueAction::Action()
{
	//set voxel
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);

	//modified voxels
	set < DataCoord > edited_voxels;

	//for all elements in the map
	map < DataCoord, OmSegID >::iterator itr;
	for (itr = mOldVoxelValues.begin(); itr != mOldVoxelValues.end(); itr++) {
		//set voxel to new value
		//cout << "Setting changed voxel" << endl;
		r_segmentation.SetVoxelValue(itr->first, mNewValue);
		edited_voxels.insert(itr->first);
	}

	//post modification
	//OmEventManager::PostEvent(new OmVoxelEvent(OmVoxelEvent::VOXEL_MODIFICATION, mSegmentationId, edited_voxels));
}

void OmVoxelSetValueAction::UndoAction()
{
	//set voxel
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);

	//modified voxels
	set < DataCoord > edited_voxels;

	//for all elements in the map
	map < DataCoord, OmSegID >::iterator itr;
	for (itr = mOldVoxelValues.begin(); itr != mOldVoxelValues.end(); itr++) {
		//set voxel to prev value
		r_segmentation.SetVoxelValue(itr->first, itr->second);
		edited_voxels.insert(itr->first);
	}

	//post modification
	//OmEventManager::PostEvent(new OmVoxelEvent(OmVoxelEvent::VOXEL_MODIFICATION, mSegmentationId, edited_voxels));
}

string OmVoxelSetValueAction::Description()
{
	string plurlize;
	if (mOldVoxelValues.size() > 1)
		plurlize = "s";

	if (NULL_SEGMENT_VALUE == mNewValue) {
		return string("Remove Voxel") + plurlize;
	} else {
		return string("Set Voxel") + plurlize;
	}
}

void OmVoxelSetValueAction::save(const string & comment)
{
	OmActionLoggerFS::save(this, comment);
}

