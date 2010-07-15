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
	void save(const string&);
	QString classNameForLogFile(){return "OmVoxelSetConnectedAction";}
	
	//dest segment id
	OmId mSegmentationId, mSegmentId;
	//dest voxel value
	OmSegID mDataValue;
	
	//source voxel
	DataCoord mSeedVoxel;
	OmId mSeedSegmentId;
	
	void AddConnectedNeighborsToList(OmSegmentation &rSegmentation, const DataCoord &srcVox, list< DataCoord >& todoList);

	friend class OmActionLoggerFS;
};

#endif
