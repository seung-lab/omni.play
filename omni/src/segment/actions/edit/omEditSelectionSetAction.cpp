
#include "omEditSelectionSetAction.h"

#include "segment/omSegmentEditor.h"


#define DEBUG 0



#pragma mark -
#pragma mark OmEditSelectionSetAction Class
/////////////////////////////////
///////
///////		 OmEditSelectionSetAction Class
///////

OmEditSelectionSetAction::OmEditSelectionSetAction(OmId segmentationId, OmId segmentId) {
	
	//store ids
	mSegmentationId = segmentationId;
	mSegmentId = segmentId; 
	
	//store current selection
	OmSegmentEditor::GetEditSelection(mOldSegmentationId, mOldSegmentId);
}





#pragma mark 
#pragma mark Action Methods
/////////////////////////////////
///////		 Action Methods


void 
OmEditSelectionSetAction::Action() {
	OmSegmentEditor::SetEditSelection(mSegmentationId, mSegmentId);
}

void 
OmEditSelectionSetAction::UndoAction()	{ 
	OmSegmentEditor::SetEditSelection(mOldSegmentationId, mOldSegmentId);
}


string 
OmEditSelectionSetAction::Description() {
	return "Change Edit Selection";
}