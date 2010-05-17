#ifndef OM_EDIT_SELECTION_SET_ACTION_H
#define OM_EDIT_SELECTION_SET_ACTION_H

#include "system/omAction.h"

class OmEditSelectionSetAction : public OmAction {

public:
	OmEditSelectionSetAction(OmId segmentationId, OmId segmentId);
	
private:
	void Action();
	void UndoAction();
	string Description();
		
	OmId mSegmentationId;
	OmId mSegmentId;
	OmId mOldSegmentationId;
	OmId mOldSegmentId;
};


#endif
