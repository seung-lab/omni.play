#include "common/omCommon.h"
#include "datalayer/fs/omActionLoggerFS.h"
#include "omVoxelSetValueAction.h"
#include "project/omProject.h"
#include "segment/actions/omSegmentEditor.h"
#include "system/omStateManager.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

/////////////////////////////////
///////
///////          OmVoxelSetValueAction Class
///////

OmVoxelSetValueAction::OmVoxelSetValueAction(OmID segmentationId,
											 DataCoord & rVoxel, OmSegID value)
{
	//store segmentation id
	mSegmentationId = segmentationId;

	//store new value
	mNewValue = value;

	//store old value of voxel
	mOldVoxelValues[rVoxel] = mNewValue;

	mUndoable = false;
}

OmVoxelSetValueAction::OmVoxelSetValueAction(OmID segmentationId,
											 std::set<DataCoord>& rVoxels,
											 OmSegID value)
{
	//store segmentation id
	mSegmentationId = segmentationId;

	//store new value
	mNewValue = value;

	//store old values of voxels
	FOR_EACH(itr, rVoxels){
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
	std::set<DataCoord> edited_voxels;

	FOR_EACH(itr, mOldVoxelValues){
		//set voxel to new value
		r_segmentation.SetVoxelValue(itr->first, mNewValue);
		edited_voxels.insert(itr->first);
	}
}

void OmVoxelSetValueAction::UndoAction()
{
	//set voxel
	OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);

	//modified voxels
	std::set<DataCoord> edited_voxels;

	FOR_EACH(itr, mOldVoxelValues){
		//set voxel to prev value
		r_segmentation.SetVoxelValue(itr->first, itr->second);
		edited_voxels.insert(itr->first);
	}
}

std::string OmVoxelSetValueAction::Description()
{
	std::string plurlize;
	if (mOldVoxelValues.size() > 1)
		plurlize = "s";

	if(0 == mNewValue) {
		return std::string("Remove Voxel") + plurlize;
	} else {
		return std::string("Set Voxel") + plurlize;
	}
}

void OmVoxelSetValueAction::save(const std::string & comment)
{
	OmActionLoggerFS::save(this, comment);
}

