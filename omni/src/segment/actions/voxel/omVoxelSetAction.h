#ifndef OM_VOXEL_SET_ACTION_H
#define OM_VOXEL_SET_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"

#include "segment/omSegmentTypes.h"
#include "volume/omVolumeTypes.h"
#include "system/omSystemTypes.h"


class OmVoxelSetAction : public OmAction {

public:
	OmVoxelSetAction(DataCoord &voxel);

	
private:
	void Action();
	void UndoAction();
	string Description();
	
	
	OmId mSegmentationId, mSegmentId;
	DataCoord mVoxel;
	SEGMENT_DATA_TYPE mPrevDataValue, mDataValue;
};



#endif
