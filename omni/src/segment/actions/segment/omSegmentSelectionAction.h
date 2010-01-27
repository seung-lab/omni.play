#ifndef OM_SEGMENT_SELECTION_ACTION_H
#define OM_SEGMENT_SELECTION_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"
#include "system/omSystemTypes.h"

class OmSegmentSelectionAction:public OmAction {

 public:
	OmSegmentSelectionAction(OmId segmentationId, OmId segmentId, bool state, bool append);

	//OmSegmentSelectionAction(OmId segmentationId, OmIds segmentIds, bool state);

 private:
	void Action();
	void UndoAction();
	string Description();

	OmId mSegmentationId, mSegmentId;

	bool mNewState;
	bool mPrevState;
	bool mAppend;

	OmIds mPrevSelectedSegments;

};

#endif
