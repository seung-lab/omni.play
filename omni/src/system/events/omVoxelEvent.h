#ifndef OM_VOXEL_EVENT_H
#define OM_VOXEL_EVENT_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 5/14/09
 */


#include "system/omEvent.h"
#include "common/omCommon.h"

class OmSegmentAction;


class OmVoxelEvent : public OmEvent {

public:
	OmVoxelEvent(QEvent::Type type);	
	OmVoxelEvent(QEvent::Type type, OmId segmentationId, DataCoord &vox); 
	OmVoxelEvent(QEvent::Type type, OmId segmentationId, set<DataCoord> &vox);
	//dispatch
	void Dispatch(OmEventListener *);
	
	//class
	static const OmEventClass CLASS  = OM_VOXEL_EVENT_CLASS;
	//events
	static const QEvent::Type VOXEL_MODIFICATION = (QEvent::Type) (CLASS + 1);
	static const QEvent::Type VOXEL_SELECTION_MODIFICATION = (QEvent::Type) (CLASS + 2);

	//accessors
	OmId GetSegmentationId();
	const set< DataCoord >& GetVoxels();
	
private:
	OmId mSegmentationId;
	set< DataCoord > mVoxels;
};



class OmVoxelEventListener : public OmEventListener {
	
public:	
	OmVoxelEventListener() : OmEventListener(OmVoxelEvent::CLASS) { };

	//voxels in a segmentation have been modified
	//valid methods: GetSegmentationId(), GetVoxels()
	virtual void VoxelModificationEvent(OmVoxelEvent * event) = 0;
	
	//the voxel selection has been modified
	virtual void VoxelSelectionModificationEvent() = 0;
};



#endif
