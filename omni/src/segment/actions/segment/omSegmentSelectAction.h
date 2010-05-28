#ifndef OM_SEGMENT_SELECT_ACTION_H
#define OM_SEGMENT_SELECT_ACTION_H

/*
 *
 *
 */

#include "system/omAction.h"

class OmSegmentSelectAction : public OmAction {

public:
	OmSegmentSelectAction(const OmId segmentationId, 
			      const OmSegIDs & selectIds, 
			      const OmSegIDs & unselectIds, 
			      const OmId segmentJustSelected,
			      void* sender,
			      const string & comment);
	
private:
	void Action();
	void UndoAction();
	string Description();
	
	const OmId mSegmentationId;
	const OmSegIDs mSelectIds;
	const OmSegIDs mUnselectIds;
	const OmId mSegmentJustSelectedID;
	void * mSender;
	const string mComment;

	OmSegIDs mModifiedSegIDs;
};

#endif
