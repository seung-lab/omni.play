#ifndef OM_VOXEL_SET_ACTION_H
#define OM_VOXEL_SET_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"



class OmVoxelSetAction : public OmAction {

public:
	OmVoxelSetAction(DataCoord &voxel);

	
private:
	void Action();
	void UndoAction();
	string Description();
	void save(const string&);
	QString classNameForLogFile(){return "OmVoxelSetAction";}
	
	OmId mSegmentationId, mSegmentId;
	DataCoord mVoxel;
	OmSegID mPrevDataValue, mDataValue;

	friend class OmActionLoggerFS;
};

#endif
