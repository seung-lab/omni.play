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
	OmVoxelSetValueAction(OmId segmentationId, std::set<DataCoord> &rVoxels,
						  OmSegID value);

private:
	void Action();
	void UndoAction();
	std::string Description();
	void save(const std::string&);
	QString classNameForLogFile(){return "OmVolxelSetvalueAction";}

	//segmentation of voxels
	OmId mSegmentationId;

	//map of voxels to old values
	map< DataCoord, OmSegID > mOldVoxelValues;

	//new value of voxels
	OmSegID mNewValue;

	template <typename T> friend class OmActionLoggerFSThread;
	friend class QDataStream &operator<<(QDataStream & out, const OmVoxelSetValueAction & a );
	friend class QDataStream &operator>>(QDataStream & in,  OmVoxelSetValueAction & a );
};

#endif
