
#include "omVoxelEvent.h"

#define DEBUG 0

OmVoxelEvent::OmVoxelEvent(QEvent::Type type)
 : OmEvent(type, CLASS)
{

}

OmVoxelEvent::OmVoxelEvent(QEvent::Type type, OmId segmentationId, DataCoord & vox)
 : OmEvent(type, CLASS)
{

	mVoxels.insert(vox);
}

OmVoxelEvent::OmVoxelEvent(QEvent::Type type, OmId segmentationId, set < DataCoord > &vox)
 : OmEvent(type, CLASS)
{

	mVoxels = vox;
}

OmId OmVoxelEvent::GetSegmentationId()
{
	return mSegmentationId;
}

const set < DataCoord > & OmVoxelEvent::GetVoxels()
{
	return mVoxels;
}

/*
 *	Dispatch event based on event type.
 */

void OmVoxelEvent::Dispatch(OmEventListener * pListener)
{

	//cast to proper listener
	OmVoxelEventListener *p_cast_listener = dynamic_cast < OmVoxelEventListener * >(pListener);
	assert(p_cast_listener);

	switch (type()) {

	case VOXEL_MODIFICATION:
		p_cast_listener->VoxelModificationEvent(this);
		return;

	case VOXEL_SELECTION_MODIFICATION:
		p_cast_listener->VoxelSelectionModificationEvent(this);
		return;

	default:
		assert(false);
	}

}
