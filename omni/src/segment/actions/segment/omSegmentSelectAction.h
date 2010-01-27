#ifndef OM_SEGMENT_SELECT_ACTION_H
#define OM_SEGMENT_SELECT_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"
#include "system/omSystemTypes.h"

class OmSegmentSelectAction:public OmAction {

 public:
	OmSegmentSelectAction(OmId segmentationId,
			      OmId segmentId, bool state, OmId segmentJustSelected = 0, void *userData = NULL);
	OmSegmentSelectAction(OmId segmentationId,
			      const OmIds & segmentIds,
			      bool state, OmId segmentJustSelected = 0, void *userData = NULL);
	 OmSegmentSelectAction(OmId segmentationId,
			       const OmIds & selectIds,
			       const OmIds & unselectIds, OmId segmentJustSelected = 0, void *userData = NULL);

 private:
	void Initialize(OmId segmentationId,
			const OmIds & selectIds,
			const OmIds & unselectIds, const OmId segmentJustSelected = 0, void *userData = NULL);
	void Action();
	void UndoAction();
	string Description();

	OmId mSegmentationId;
	OmIds mSelectIds;
	OmIds mUnselectIds;

	OmId mSegmentJustSelectedID;
	void *mUserData;

	bool mNewState;
	 map < OmId, bool > mPrevSegmentStates;
};

#endif
