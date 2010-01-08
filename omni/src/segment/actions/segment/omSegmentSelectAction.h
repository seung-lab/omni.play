#ifndef OM_SEGMENT_SELECT_ACTION_H
#define OM_SEGMENT_SELECT_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"
#include "system/omSystemTypes.h"



class OmSegmentSelectAction : public OmAction {

public:
	OmSegmentSelectAction(OmId segmentationId, OmId segmentId, bool state);
	OmSegmentSelectAction(OmId segmentationId, const OmIds &segmentIds, bool state);
	OmSegmentSelectAction(OmId segmentationId, const OmIds &selectIds, const OmIds &unselectIds);
	
private:
	void Initialize(OmId segmentationId, const OmIds &selectIds, const OmIds &unselectIds);
	void Action();
	void UndoAction();
	string Description();
	
	
	OmId mSegmentationId;
	OmIds mSelectIds;
	OmIds mUnselectIds;
	
	bool mNewState;
	map< OmId, bool > mPrevSegmentStates;
};



#endif
