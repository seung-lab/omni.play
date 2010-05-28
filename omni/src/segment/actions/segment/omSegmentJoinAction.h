#ifndef OM_SEGMENT_JOIN_ACTION_H
#define OM_SEGMENT_JOIN_ACTION_H

/*
 *
 *
 */
#include "system/omAction.h"

class OmSegmentJoinAction : public OmAction {

public:
	OmSegmentJoinAction(OmId segmentationId, OmIds selectedSegmentIds);


private:
	void Action();
	void UndoAction();
	string Description();

	OmIds mSelectedSegmentIds;
	OmId mSegmentationId;

};



#endif
