#include "segment/omSegmentSelector.h"
#include "segment/omSegmentUtils.hpp"
#include "actions/details/omProjectSaveAction.h"
#include "actions/details/omProjectCloseAction.h"
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
#include "system/omLocalPreferences.h"

// project-related
void OmActions::Save(){
	(new OmProjectSaveAction())->Run();
}

void OmActions::Close(){
	(new OmProjectCloseAction())->Run();
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
								const om::SetValid valid)
{
	const OmSegID nextSegmentIDtoJumpTo =
		OmSegmentUtils::GetNextSegIDinWorkingList(sdw);

	OmSegmentValidateAction::Validate(sdw, valid);

	OmSegmentCache* segCache = sdw.GetSegmentCache();

	const bool segmentGettingSetAsValid = om::SET_VALID == valid;
	const bool shouldJump =
		OmLocalPreferences::GetShouldJumpToNextSegmentAfterValidate();
	const bool justOneSegmentSelected =
		(1 == segCache->GetSelectedSegmentIds().size());

	if( justOneSegmentSelected &&
		segmentGettingSetAsValid &&
		shouldJump &&
		nextSegmentIDtoJumpTo)
	{
		OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(),
							  NULL,
							  "jump after validate");
		sel.selectJustThisSegment(nextSegmentIDtoJumpTo, true);
		sel.sendEvent();
	}
}

void OmActions::ValidateSelectedSegments(const SegmentationDataWrapper& sdw,
										 const om::SetValid valid)
{
	const OmSegID nextSegmentIDtoJumpTo =
		OmSegmentUtils::GetNextSegIDinWorkingList(sdw);

	OmSegmentValidateAction::Validate(sdw, valid);

	const bool segmentGettingSetAsValid = om::SET_VALID == valid;
	const bool shouldJump =
		OmLocalPreferences::GetShouldJumpToNextSegmentAfterValidate();

	if(shouldJump &&
	   segmentGettingSetAsValid &&
	   nextSegmentIDtoJumpTo)
	{
		OmSegmentSelector sel(sdw, NULL, "jump after validate");
		sel.selectJustThisSegment(nextSegmentIDtoJumpTo, true);
		sel.sendEvent();
	}
}

void OmActions::UncertainSegment(const SegmentDataWrapper& sdw,
								 const bool uncertain){
        bool dontjump = false;
        bool jump = OmLocalPreferences::GetShouldJumpToNextSegmentAfterValidate();
        OmSegID seg = sdw.GetSegmentation().GetSegmentLists()->Working().GetNextSegmentIDinList(sdw.getSegment()->getRootSegID());

	OmSegmentUncertainAction::SetUncertain(sdw, uncertain);

        OmSegmentIterator iter(sdw.GetSegmentCache());
        iter.iterOverSelectedIDs();
        OmSegment * segment = iter.getNextSegment();
        segment = iter.getNextSegment();

        if(jump && NULL != segment) {
                dontjump = true;
        }

        if(!dontjump && uncertain && jump && seg && sdw.GetSegmentCache()->IsSegmentValid(seg)) {
                sdw.GetSegmentCache()->SetAllSelected(false);
                sdw.GetSegmentCache()->setSegmentSelected(seg, true, true);
        }
}

void OmActions::UncertainSegment(const SegmentationDataWrapper& sdw,
								 const bool uncertain){
        bool jump = OmLocalPreferences::GetShouldJumpToNextSegmentAfterValidate();

        OmSegmentIterator iter(sdw.GetSegmentCache());
        iter.iterOverSelectedIDs();
        OmSegment * segment = iter.getNextSegment();

        OmSegID seg = 0;
        while(jump && NULL != segment) {
                seg = sdw.GetSegmentation().GetSegmentLists()->Working().GetNextSegmentIDinList(segment->getRootSegID());
                if(seg) {
                        break;
                }
                segment = iter.getNextSegment();
        }

	OmSegmentUncertainAction::SetUncertain(sdw, uncertain);

        if(uncertain && jump && seg && sdw.GetSegmentCache()->IsSegmentValid(seg)) {
                sdw.GetSegmentCache()->SetAllSelected(false);
                sdw.GetSegmentCache()->setSegmentSelected(seg, true, true);
        }
}


OmSegIDsSet OmActions::MutateSegmentsInValidList(OmSegmentCache * cache, const OmSegIDsSet& ids)
{
        OmSegIDsSet ret;

        FOR_EACH(iter, ids){
                OmSegment* seg = cache->GetSegment(*iter);
                if(!seg){
                        continue;
                }
                if(seg->IsValidListType()){
                        OmActions::ValidateSegment(SegmentDataWrapper(seg), om::SET_NOT_VALID);
                        ret.insert(seg->value());
                }
        }
        return ret;
}

void OmActions::UnMutateSegmentsInValidList(OmSegmentCache * cache, const OmSegIDsSet& ids)
{
        FOR_EACH(iter, ids){
                OmSegment* seg = cache->GetSegment(*iter);
                if(!seg){
                        continue;
                }
                OmActions::ValidateSegment(SegmentDataWrapper(seg), om::SET_VALID);
        }
}

void OmActions::JoinSegments(const OmID segmentationID,
							 const OmSegIDsSet& ids)
{
	SegmentationDataWrapper sdw(segmentationID);
	bool revalidate = false;
	OmSegIDsSet validSet;

	if(sdw.GetSegmentCache()->AreAnySegmentsInValidList(ids)){
		//printf("valid segment present in list; not joining...\n");
		//return;		// don't alow the join if valid segment is given
		revalidate = true;
		validSet = MutateSegmentsInValidList(sdw.GetSegmentCache(), ids);
	}

	(new OmSegmentJoinAction(segmentationID, ids))->Run();

	if(revalidate) {
		UnMutateSegmentsInValidList(sdw.GetSegmentCache(), validSet);
	}
}

void OmActions::FindAndSplitSegments(const SegmentDataWrapper& sdw,
									 OmViewGroupState* vgs){
	OmSegmentSplitAction::DoFindAndSplitSegment(sdw, vgs);
}

void OmActions::FindAndCutSegments(const SegmentDataWrapper& sdw,
									 OmViewGroupState* vgs){
	OmSegmentSplitAction::DoFindAndCutSegment(sdw, vgs);
}

void OmActions::SelectSegments(const SegmentDataWrapper& sdw,
							   const OmSegIDsSet & mNewSelectedIdSet,
							   const OmSegIDsSet & mOldSelectedIdSet,
							   void* sender,
							   const std::string & comment,
							   const bool doScroll,
							   const bool addToRecentList)
{
	(new OmSegmentSelectAction(sdw,
							   mNewSelectedIdSet,
							   mOldSelectedIdSet,
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
