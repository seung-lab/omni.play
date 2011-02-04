#include "actions/omSelectSegmentParams.hpp"
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
#include "actions/omActions.h"
#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "system/omLocalPreferences.hpp"

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
                                const om::SetValid valid, const bool dontCenter)
{
    const OmSegID nextSegmentIDtoJumpTo =
        OmSegmentUtils::GetNextSegIDinWorkingList(sdw);

    OmSegmentValidateAction::Validate(sdw, valid);

    OmSegmentCache* segCache = sdw.SegmentCache();

    const bool segmentGettingSetAsValid = om::SET_VALID == valid;
    const bool shouldJump =
        OmLocalPreferences::GetShouldJumpToNextSegmentAfterValidate();
    const bool justOneSegmentSelected =
        (1 == segCache->GetSelectedSegmentIds().size());

    if( justOneSegmentSelected &&
        segmentGettingSetAsValid &&
        shouldJump &&
        nextSegmentIDtoJumpTo &&
        !dontCenter)
    {
        OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(),
                              NULL,
                              "jump after validate");
        sel.selectJustThisSegment(nextSegmentIDtoJumpTo, true);
        sel.AutoCenter(true);
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
        sel.AutoCenter(true);
        sel.sendEvent();
    }
}

void OmActions::UncertainSegment(const SegmentDataWrapper& sdw,
                                 const bool uncertain){
    bool shouldJump = OmLocalPreferences::GetShouldJumpToNextSegmentAfterValidate();
    const OmSegID nextSegmentIDtoJumpTo =
        OmSegmentUtils::GetNextSegIDinWorkingList(sdw);

    OmSegmentUncertainAction::SetUncertain(sdw, uncertain);

    if(shouldJump &&
       uncertain &&
       nextSegmentIDtoJumpTo)
    {
        OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), NULL, "jump after validate");
        sel.selectJustThisSegment(nextSegmentIDtoJumpTo, true);
        sel.AutoCenter(true);
        sel.sendEvent();
    }
}

void OmActions::UncertainSegment(const SegmentationDataWrapper& sdw,
                                 const bool uncertain){
    bool shouldJump = OmLocalPreferences::GetShouldJumpToNextSegmentAfterValidate();
    const OmSegID nextSegmentIDtoJumpTo =
        OmSegmentUtils::GetNextSegIDinWorkingList(sdw);

    OmSegmentUncertainAction::SetUncertain(sdw, uncertain);

    if(shouldJump &&
       uncertain &&
       nextSegmentIDtoJumpTo)
    {
        OmSegmentSelector sel(sdw, NULL, "jump after validate");
        sel.selectJustThisSegment(nextSegmentIDtoJumpTo, true);
        sel.AutoCenter(true);
        sel.sendEvent();
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
        OmActions::ValidateSegment(SegmentDataWrapper(seg), om::SET_VALID, true);
    }
}

void OmActions::JoinSegments(const OmID segmentationID,
                             const OmSegIDsSet& ids)
{
    SegmentationDataWrapper sdw(segmentationID);
    bool revalidate = false;
    OmSegIDsSet validSet;

    if(sdw.SegmentCache()->AreAnySegmentsInValidList(ids)){
        //printf("valid segment present in list; not joining...\n");
        //return;		// don't alow the join if valid segment is given
        revalidate = true;
        validSet = MutateSegmentsInValidList(sdw.SegmentCache(), ids);
    }

    (new OmSegmentJoinAction(segmentationID, ids))->Run();

    if(revalidate) {
        UnMutateSegmentsInValidList(sdw.SegmentCache(), validSet);
    }
}

void OmActions::DoFindAndSplitSegment(const SegmentDataWrapper& sdw, OmViewGroupState* vgs, const DataCoord coord)
{
    const std::pair<boost::optional<DataCoord>, SegmentDataWrapper> splittingAndSDW =
        vgs->GetSplitMode();
    const boost::optional<DataCoord> amAlreadySplitting = splittingAndSDW.first;

    if(amAlreadySplitting){

        SegmentDataWrapper segmentBeingSplit = splittingAndSDW.second;

        OmSegment* seg1 = segmentBeingSplit.getSegment();
        OmSegment* seg2 = sdw.getSegment();

        if(NULL == seg1 || NULL == seg2) {
            return;
        }

        OmSegmentSplitAction::RunIfSplittable(seg1, seg2, *amAlreadySplitting, coord);

        vgs->SetSplitMode(false);

    } else { // set segment to be split later...
        if(sdw.IsSegmentValid()){
            vgs->SetSplitMode(sdw, coord);
        }
    }
}

void OmActions::FindAndSplitSegments(const SegmentDataWrapper& sdw,
                                     OmViewGroupState* vgs,
                                     const DataCoord dataClickPoint){
    DoFindAndSplitSegment(sdw, vgs, dataClickPoint);
}

void OmActions::FindAndCutSegments(const SegmentDataWrapper& sdw,
                                   OmViewGroupState* vgs){
    OmSegmentSplitAction::DoFindAndCutSegment(sdw, vgs);
}

void OmActions::SelectSegments(boost::shared_ptr<OmSelectSegmentsParams> params){
    (new OmSegmentSelectAction(params))->Run();
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
