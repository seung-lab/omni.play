#ifndef OM_VOXEL_SELECTION_ACTION_H
#define OM_VOXEL_SELECTION_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"



class OmVoxelSelectionAction : public OmAction {

public:
	OmVoxelSelectionAction(const DataCoord &voxel, bool state, bool append);
	
private:	
	void Action();
	void UndoAction();
	string Description();
	
	DataCoord mVoxel;
	
	bool mNewState;
	bool mOldState;
	bool mAppend;

	//set< DataCoord > mVoxels;
	map< DataCoord, bool > mVoxelsToPrevStateMap;
	set< DataCoord > mPrevSelectedVoxels;

};



#endif
