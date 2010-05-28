#ifndef OM_SEGMENT_SELECT_ACTION_H
#define OM_SEGMENT_SELECT_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"

class SegmentDataWrapper;

class OmSegmentSelectAction : public OmAction {

public:
	OmSegmentSelectAction(OmId segmentationId, 
			      const OmSegIDs & selectIds, 
			      const OmSegIDs & unselectIds, 
			      OmId segmentJustSelected,
			      void* sender,
			      string comment);
	
private:
	void Action();
	void UndoAction();
	string Description();
	
	OmId mSegmentationId;
	OmSegIDs mSelectIds;
	OmSegIDs mUnselectIds;
	OmSegIDs modifiedSegIDs;

	OmId mSegmentJustSelectedID;
	void* mSender;
	string mComment;
};

#endif
