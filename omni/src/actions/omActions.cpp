#include "actions/details/omProjectSaveAction.h"
#include "actions/details/omSegmentGroupAction.h"
#include "actions/details/omSegmentJoinAction.h"
#include "actions/details/omSegmentSelectAction.h"
#include "actions/details/omSegmentSplitAction.h"
#include "actions/details/omSegmentUncertainAction.h"
#include "actions/details/omSegmentValidateAction.h"
#include "actions/details/omSegmentationThresholdChangeAction.h"
#include "actions/details/omVoxelSetValueAction.h"
#include "actions/omActions.hpp"
#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "segment/omSegmentLists.hpp"
#include "segment/omSegmentIterator.h"

// project-related
void OmActions::Save(){
	(new OmProjectSaveAction())->Run();
}

// MST-related
void OmActions::ChangeMSTthreshold(const OmID segmentationID,
								   const float threshold)
{
	(new OmSegmentationThresholdChangeAction(segmentationID,
											 threshold))->Run();
}

//painting-related
void OmActions::SetVoxel(const OmID segmentationID,
						 const DataCoord& voxel,
						 const OmSegID segmentID)
{
	(new OmVoxelSetValueAction(segmentationID,
							   voxel,
							   segmentID))->Run();
}

void OmActions::SetVoxels(const OmID segmentationID,
						  const std::set<DataCoord>& voxels,
						  const OmSegID segmentID)
{
	(new OmVoxelSetValueAction(segmentationID,
							   voxels,
							   segmentID))->Run();
}

// segment-related
void OmActions::ValidateSegment(const SegmentDataWrapper& sdw,
								const om::SetValid valid){
	OmSegID seg = sdw.getSegmentation().GetSegmentLists()->Working().GetNextSegmentIDinList(sdw.getSegment()->getRootSegID());
	OmSegmentValidateAction::Validate(sdw, valid);
	if(seg) {
		sdw.getSegmentation().GetSegmentCache()->SetAllSelected(false);
		sdw.getSegmentation().GetSegmentCache()->setSegmentSelected(seg, true, true);
	}
}

void OmActions::ValidateSegment(const SegmentationDataWrapper& sdw,
								const om::SetValid valid){
	OmSegmentIterator iter(sdw.getSegmentation().GetSegmentCache());
	iter.iterOverSelectedIDs();
	OmSegment * segment = iter.getNextSegment();
	OmSegID seg;
	while(NULL != segment) {
		seg = sdw.getSegmentation().GetSegmentLists()->Working().GetNextSegmentIDinList(segment->getRootSegID());
		if(seg) {
			break;
		}
                segment = iter.getNextSegment();
        }

	OmSegmentValidateAction::Validate(sdw, valid);

	if(seg) {
		sdw.getSegmentation().GetSegmentCache()->SetAllSelected(false);
		sdw.getSegmentation().GetSegmentCache()->setSegmentSelected(seg, true, true);
	}
}

void OmActions::UncertainSegment(const SegmentDataWrapper& sdw,
								 const bool uncertain){
	OmSegmentUncertainAction::SetUncertain(sdw, uncertain);
}

void OmActions::UncertainSegment(const SegmentationDataWrapper& sdw,
								 const bool uncertain){
	OmSegmentUncertainAction::SetUncertain(sdw, uncertain);
}

void OmActions::JoinSegments(const OmID segmentationID,
							 const OmSegIDsSet& ids)
{
	SegmentationDataWrapper sdw(segmentationID);
	if(sdw.GetSegmentLists()->AreAnySegmentsInValidList(ids)){
		printf("valid segment present in list; not joining...\n");
		return;		// don't alow the join if valid segment is given
	}

	(new OmSegmentJoinAction(segmentationID, ids))->Run();
}

void OmActions::FindAndSplitSegments(const SegmentDataWrapper& sdw,
									 OmViewGroupState* vgs){
	OmSegmentSplitAction::DoFindAndSplitSegment(sdw, vgs);
}

void OmActions::SelectSegments(const OmID segmentationId,
							   const OmSegIDsSet & mNewSelectedIdSet,
							   const OmSegIDsSet & mOldSelectedIdSet,
							   const OmID segmentJustSelected,
							   void* sender,
							   const std::string & comment,
							   const bool doScroll,
							   const bool addToRecentList)
{
	(new OmSegmentSelectAction(segmentationId,
							   mNewSelectedIdSet,
							   mOldSelectedIdSet,
							   segmentJustSelected,
							   sender,
							   comment,
							   doScroll,
							   addToRecentList))->Run();
}

// group-related
void OmActions::CreateOrDeleteSegmentGroup(const OmID segmentationID,
										   const OmSegIDsSet& selectedSegmentIDs,
										   const OmGroupName name,
										   const bool create)
{
	(new OmSegmentGroupAction(segmentationID,
							  selectedSegmentIDs,
							  name,
							  create))->Run();
}
