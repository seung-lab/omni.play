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
			      const OmSegIDs & mNewSelectedIdSet, 
			      const OmSegIDs & mOldSelectedIdSet, 
			      const OmId segmentJustSelected,
			      void* sender,
			      const string & comment);
	
private:
	void Action();
	void UndoAction();
	string Description();
	
	const OmId mSegmentationId;
	const OmSegIDs mNewSelectedIdSet;
	const OmSegIDs mOldSelectedIdSet;
	const OmId mSegmentJustSelectedID;
	void * mSender;
	const string mComment;
};

#endif
