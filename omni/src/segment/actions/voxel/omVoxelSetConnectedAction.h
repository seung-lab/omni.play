#ifndef OM_VOXEL_SET_CONNECTED_ACTION_H
#define OM_VOXEL_SET_CONNECTED_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"




#include <list>
using std::list;


class OmSegmentation;



class OmVoxelSetConnectedAction : public OmAction {

public:
	OmVoxelSetConnectedAction();

	
private:
	void Action();
	void UndoAction();
	string Description();
	
	//dest segment id
	OmId mSegmentationId, mSegmentId;
	//dest voxel value
	SEGMENT_DATA_TYPE mDataValue;
	
	//source voxel
	DataCoord mSeedVoxel;
	OmId mSeedSegmentId;
	
	void AddConnectedNeighborsToList(OmSegmentation &rSegmentation, const DataCoord &srcVox, list< DataCoord >& todoList);
};



#endif
