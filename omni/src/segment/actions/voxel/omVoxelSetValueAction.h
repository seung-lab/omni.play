#ifndef OM_VOXEL_SET_VALUE_ACTION_H
#define OM_VOXEL_SET_VALUE_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"



class OmVoxelSetValueAction : public OmAction {

public:
	OmVoxelSetValueAction(OmId segmentationId, DataCoord &rVoxel, OmSegID value);
	OmVoxelSetValueAction(OmId segmentationId, set<DataCoord> &rVoxels, OmSegID value);

	
private:
	void Action();
	void UndoAction();
	string Description();
	
	//segmentation of voxels
	OmId mSegmentationId;
	
	//map of voxels to old values
	map< DataCoord, OmSegID > mOldVoxelValues;
	//new value of voxels
	OmSegID mNewValue;
};



#endif
