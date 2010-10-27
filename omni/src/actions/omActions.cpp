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
								const bool valid){
	OmSegmentValidateAction::Validate(sdw, valid);
}

void OmActions::ValidateSegment(const SegmentationDataWrapper& sdw,
								const bool valid){
	OmSegmentValidateAction::Validate(sdw, valid);
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
							 const OmSegIDsSet& ids){
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
