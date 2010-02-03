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
	OmSegmentSelectAction(OmId segmentationId, 
			      OmId segmentId, 
			      bool state, 
			      OmId segmentJustSelected = 0);

	OmSegmentSelectAction(OmId segmentationId, 
			      const OmIds &segmentIds, 
			      bool state, 
			      OmId segmentJustSelected = 0);

	OmSegmentSelectAction(OmId segmentationId, 
			      const OmIds &selectIds, 
			      const OmIds &unselectIds, 
			      OmId segmentJustSelected = 0,
			      void* sender = 0,
			      string comment = "");
	
private:
	void Initialize(OmId segmentationId, 
			const OmIds &selectIds, 
			const OmIds &unselectIds,
			const OmId segmentJustSelected = 0,
			void* sender = NULL,
			string comment = "" );
	void Action();
	void UndoAction();
	string Description();
	
	
	OmId mSegmentationId;
	OmIds mSelectIds;
	OmIds mUnselectIds;
	
	OmId mSegmentJustSelectedID;
	void* mSender;
	string mComment;

	bool mNewState;
	map< OmId, bool > mPrevSegmentStates;
};



#endif
