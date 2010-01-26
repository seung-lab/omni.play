
#include "omSegmentContextMenu.h"

#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/actions/segment/omSegmentStateAction.h"
#include "segment/actions/segment/omSegmentMergeAction.h"
#include "segment/actions/voxel/omVoxelSetConnectedAction.h"
#include "segment/actions/edit/omEditSelectionSetAction.h"

#include "volume/omVolume.h"
#include "volume/omSegmentation.h"

#include "utility/setUtilities.h"
#include "system/omDebug.h"

#define DEBUG 0



#pragma mark -
#pragma mark Example Class
/////////////////////////////////
///////
///////		 Example Class
///////



#pragma mark 
#pragma mark Example Methods
/////////////////////////////////
///////		 Example Methods




#pragma mark 
#pragma mark Context Menu Methods
/////////////////////////////////
///////		 Context Menu Methods

void 
OmSegmentContextMenu::Refresh(OmId segmentationId, OmId segmentId) {
	
	//store ids
	mSegmentationId = segmentationId;
	mSegmentId = segmentId;
	
	
	//clear old menu actions
	clear();
	
		
	//SELECTION NAMES
	AddSelectionNames();
	
	//-------------
	addSeparator();
	
	//UN/SELECT SEGMENT
	AddSelectionAction();
	
	//-------------
	addSeparator();
	
	//DISABLE
	AddDisableActions();
	
	//SELECT ALL
	//AddAllSelectionAction();
	
	/*
	//-------------
	addSeparator();
	addAction( QString("Copy Segment") );
	addAction( QString("Paste Segment") );	
	addAction( QString("Delete Segment") );
	 */
	//-------------
	 addSeparator();
	
	AddEditSelectionAction();
	
	AddMergeAction();

	AddVoxelAction();
	
	/*
	 sendBackAction = new QAction(QIcon(":/images/sendtoback.png"),
	 tr("Send to &Back"), this);
	 sendBackAction->setShortcut(tr("Ctrl+B"));
	 sendBackAction->setStatusTip(tr("Send item to back"));
	 connect(sendBackAction, SIGNAL(triggered()),
	 this, SLOT(sendToBack()));
	 */
}







void 
OmSegmentContextMenu::AddSelectionNames() {
	//get segmentation and segment
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);
	OmSegment &r_segment = r_segmentation.GetSegment(mSegmentId);

	string segmentatioin_str =  r_segmentation.GetName();
	string segment_str = r_segment.GetName();
	
	addAction( QString(segment_str.c_str()) );
	addAction( QString(segmentatioin_str.c_str()) );
	
}







/*
 *	Adds Un/Select Segment Action
 */
void 
OmSegmentContextMenu::AddSelectionAction() {
	
	//get segmentation and segment
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);
	
	//is segment selected
	bool is_segment_selected = r_segmentation.IsSegmentSelected(mSegmentId);
	
	//if segment is already selected
	if( is_segment_selected ) {
		addAction( QString("Unselect Segment"), this, SLOT(Unselect()));
	} else {
		addAction( QString("Select Segment"), this, SLOT(Select()));
	}

	
	addAction( QString("Select All") );
}






/*
 *	Adds Disable Segment Actions
 */
void
OmSegmentContextMenu::AddDisableActions() {
	
	//is segment selected
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);
	bool is_segment_enabled = r_segmentation.IsSegmentEnabled(mSegmentId);
	
	//if segment is already selected
	if( is_segment_enabled ) {
		string action_str = string("Disable Segment");
		addAction( QString(action_str.c_str()), this, SLOT(Disable()));
		
	} else {
		string action_str = string("Enable Segment");
		addAction( QString("Enable Segment"), this, SLOT(Enable()));
	}

	//addAction( QString("Disable Selected"), this, SLOT(DisableSelected()));
	
	addAction( QString("Disable Unselected"), this, SLOT(DisableUnselected()));
	
	addAction( QString("Enable All"), this, SLOT(EnableAll()));
}






void 
OmSegmentContextMenu::AddEditSelectionAction() {
	addAction( QString("Set Edit Selection"), this, SLOT(SetEditSelection()));
}





/*
 *	Merge Segments
 */
void 
OmSegmentContextMenu::AddMergeAction() {
	addAction( QString("Merge Selected"), this, SLOT(MergeSegments()));
}




/*
 *	Merge Segments
 */
void 
OmSegmentContextMenu::AddVoxelAction() {
	addAction( QString("Set Connected Voxels"), this, SLOT(SetConnectedVoxels()));
}










#pragma mark 
#pragma mark Context Menu Slots Methods
/////////////////////////////////
///////		 Context Menu Slots Methods



void
OmSegmentContextMenu::Select() {
	(new OmSegmentSelectAction(mSegmentationId, mSegmentId, true))->Run();
}

void
OmSegmentContextMenu::Unselect() {
	(new OmSegmentSelectAction(mSegmentationId, mSegmentId, false))->Run();
}

void
OmSegmentContextMenu::UnselectOthers() {
	
	//get current selection
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);
	OmIds selected_segment_ids = r_segmentation.GetSelectedSegmentIds();
	selected_segment_ids.erase(mSegmentId);

	(new OmSegmentSelectAction(mSegmentationId, selected_segment_ids, false))->Run();
}






void
OmSegmentContextMenu::Disable() {
	(new OmSegmentStateAction(mSegmentationId, mSegmentId, false))->Run();
}
void
OmSegmentContextMenu::Enable() {
	(new OmSegmentStateAction(mSegmentationId, mSegmentId, true))->Run();
}


void
OmSegmentContextMenu::DisableSelected() {
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);
	(new OmSegmentStateAction(mSegmentationId, r_segmentation.GetSelectedSegmentIds(), false))->Run();
}
void
OmSegmentContextMenu::DisableUnselected() {
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//form unselected
	OmIds unselected_segments;
	setDifference< OmId >(r_segmentation.GetEnabledSegmentIds(), 
						  r_segmentation.GetSelectedSegmentIds(), 
						  unselected_segments);
	
	(new OmSegmentStateAction(mSegmentationId, unselected_segments, false))->Run();
}
void
OmSegmentContextMenu::EnableAll() {
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);
	(new OmSegmentStateAction(mSegmentationId, r_segmentation.GetValidSegmentIds(), true))->Run();
}






void 
OmSegmentContextMenu::SetEditSelection() {
	(new OmEditSelectionSetAction(mSegmentationId, mSegmentId))->Run();
}


void
OmSegmentContextMenu::MergeSegments() {
	(new OmSegmentMergeAction(mSegmentationId))->Run();
}



void
OmSegmentContextMenu::SetConnectedVoxels() {
	(new OmVoxelSetConnectedAction())->Run();
}




