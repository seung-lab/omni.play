#ifndef OM_VOXEL_SET_VALUE_ACTION_H
#define OM_VOXEL_SET_VALUE_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"

#include "segment/omSegmentTypes.h"
#include "volume/omVolumeTypes.h"
#include "system/omSystemTypes.h"

class OmVoxelSetValueAction:public OmAction {

 public:
	OmVoxelSetValueAction(OmId segmentationId, DataCoord & rVoxel, SEGMENT_DATA_TYPE value);
	OmVoxelSetValueAction(OmId segmentationId, set < DataCoord > &rVoxels, SEGMENT_DATA_TYPE value);

 private:
	void Action();
	void UndoAction();
	string Description();

	//segmentation of voxels
	OmId mSegmentationId;

	//map of voxels to old values
	 map < DataCoord, SEGMENT_DATA_TYPE > mOldVoxelValues;
	//new value of voxels
	SEGMENT_DATA_TYPE mNewValue;
};

#endif
