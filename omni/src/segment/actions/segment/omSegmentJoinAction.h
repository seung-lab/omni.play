#ifndef OM_SEGMENT_JOIN_ACTION_H
#define OM_SEGMENT_JOIN_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"

class OmSegmentJoinAction : public OmAction {

public:
	OmSegmentJoinAction( const OmId segmentationId, const OmSegIDsSet & selectedSegmentIds);

private:
	void Action();
	void UndoAction();
	string Description();

	const OmId mSegmentationId;
	OmSegIDsSet mSelectedSegmentIds;
};

#endif
