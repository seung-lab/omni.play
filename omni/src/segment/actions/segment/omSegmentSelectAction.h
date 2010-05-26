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

	void selectJustThisSegment( SegmentDataWrapper sdw );
	void addSegmentToSelectedSet( SegmentDataWrapper sdw );
	OmSegmentSelectAction();
	void commitChanges();


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
	OmIds ssegmentsToAddToSelection;

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
};



#endif
