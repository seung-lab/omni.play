#ifndef OM_SEGMENT_STATE_ACTION_H
#define OM_SEGMENT_STATE_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"
#include "system/omSystemTypes.h"



class OmSegmentStateAction : public OmAction {

public:
	OmSegmentStateAction(OmId segmentationId, OmId segmentId, bool state);
	OmSegmentStateAction(OmId segmentationId, const OmIds &segmentIds, bool state);
	
private:
	void Initialize(OmId segmentationId, const OmIds &segmentIds, bool state);
	void Action();
	void UndoAction();
	string Description();
	
	
	OmId mSegmentationId;
	OmIds mSegmentIds;
	
	bool mNewState;
	map< OmId, bool > mPrevSegmentStates;
};



#endif
