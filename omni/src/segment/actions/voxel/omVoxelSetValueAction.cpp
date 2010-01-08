
#include "omVoxelSetValueAction.h"

#include "segment/omSegmentEditor.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

#include "system/omStateManager.h"
#include "system/omEventManager.h"
#include "system/events/omVoxelEvent.h"

#define DEBUG 0



#pragma mark -
#pragma mark OmVoxelSetValueAction Class
/////////////////////////////////
///////
///////		 OmVoxelSetValueAction Class
///////


OmVoxelSetValueAction::OmVoxelSetValueAction(OmId segmentationId, DataCoord &rVoxel, SEGMENT_DATA_TYPE value) {
	
	//store segmentation id
	mSegmentationId = segmentationId;

	//store new value
	mNewValue = value;
	
	//get segmentation
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);
	
	//store old value of voxel
	//mOldVoxelValues[rVoxel] = r_segmentation.GetVoxelValue(rVoxel);
	mOldVoxelValues[rVoxel] = mNewValue;

	mUndoable = false;
}


OmVoxelSetValueAction::OmVoxelSetValueAction(OmId segmentationId, set<DataCoord> &rVoxels, SEGMENT_DATA_TYPE value) {
	
	//store segmentation id
	mSegmentationId = segmentationId;
	
	//store new value
	mNewValue = value;
	
	//get segmentation
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);
	
#if 1
	//store old values of voxels
	set<DataCoord>::iterator itr;
	for(itr = rVoxels.begin(); itr != rVoxels.end(); itr++) {
		//mOldVoxelValues[*itr] = r_segmentation.GetVoxelValue(*itr);
		mOldVoxelValues[*itr] = mNewValue;
	}
#endif

	mUndoable = false;
}







#pragma mark 
#pragma mark Action Methods
/////////////////////////////////
///////		 Action Methods


void 
OmVoxelSetValueAction::Action() {
	//set voxel
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);
	
	//modified voxels
	set< DataCoord > edited_voxels;
	
	//for all elements in the map
	map< DataCoord, SEGMENT_DATA_TYPE >::iterator itr;
	for( itr = mOldVoxelValues.begin(); itr != mOldVoxelValues.end(); itr++) {
		//set voxel to new value
		//cout << "Setting changed voxel" << endl;
		r_segmentation.SetVoxelValue(itr->first, mNewValue);
		edited_voxels.insert(itr->first);
	}
	
	//post modification
	OmEventManager::PostEvent(new OmVoxelEvent(OmVoxelEvent::VOXEL_MODIFICATION, mSegmentationId, edited_voxels));
}



void 
OmVoxelSetValueAction::UndoAction()	{ 
	//set voxel
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//modified voxels
	set< DataCoord > edited_voxels;
	
	//for all elements in the map
	map< DataCoord, SEGMENT_DATA_TYPE >::iterator itr;
	for( itr = mOldVoxelValues.begin(); itr != mOldVoxelValues.end(); itr++) {
		//set voxel to prev value
		r_segmentation.SetVoxelValue(itr->first, itr->second);
		edited_voxels.insert(itr->first);
	}

	//post modification
	OmEventManager::PostEvent(new OmVoxelEvent(OmVoxelEvent::VOXEL_MODIFICATION, mSegmentationId, edited_voxels));
}



string 
OmVoxelSetValueAction::Description() {

	string plurlize;
	if(mOldVoxelValues.size() > 1)
		plurlize = "s";
	
	
	if(NULL_SEGMENT_DATA == mNewValue) {
		return string("Remove Voxel") + plurlize;
	} else {
		return string("Set Voxel") + plurlize;
	}
}
