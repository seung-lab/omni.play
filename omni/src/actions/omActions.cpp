#include "segment/omFindCommonEdge.hpp"
#include "actions/omSelectSegmentParams.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegmentUtils.hpp"

#include "actions/details/omActionImpls.hpp"
#include "actions/details/omSegmentSplitAction.h"
#include "actions/details/omSegmentUncertainAction.h"
#include "actions/details/omSegmentValidateAction.h"
#include "actions/details/omVoxelSetValueAction.h"

#include "actions/omActions.h"
#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "system/omLocalPreferences.hpp"
#include "viewGroup/omSplitting.hpp"

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

    setUncertain(sdw, uncertain);

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

    setUncertain(sdw, uncertain);

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

void OmActions::setUncertain(const SegmentDataWrapper& sdw,
                             const bool uncertain)
{
    OmSegIDsSet set;
    set.insert(sdw.FindRootID());

    boost::shared_ptr<std::set<OmSegment*> > children =
        OmSegmentUtils::GetAllChildrenSegments(sdw.SegmentCache(), set);

    (new OmSegmentUncertainAction(sdw.MakeSegmentationDataWrapper(),
                                  children, uncertain))->Run();
}

void OmActions::setUncertain(const SegmentationDataWrapper& sdw,
                             const bool uncertain)
{
    OmSegmentCache* segCache = sdw.SegmentCache();
    boost::shared_ptr<std::set<OmSegment*> > children =
        OmSegmentUtils::GetAllChildrenSegments(segCache,
                                               segCache->GetSelectedSegmentIds());

    (new OmSegmentUncertainAction(sdw, children, uncertain))->Run();
}

OmSegPtrSet OmActions::setNotValid(OmSegmentCache* cache, const OmSegIDsSet& ids)
{
    OmSegPtrSet ret;

    FOR_EACH(iter, ids){
        OmSegment* seg = cache->GetSegment(*iter);
        if(!seg){
            continue;
        }
        if(seg->IsValidListType()){
            OmActions::ValidateSegment(SegmentDataWrapper(seg), om::SET_NOT_VALID);
            ret.insert(seg);
        }
    }
    return ret;
}

void OmActions::setAsValid(const OmSegPtrSet& segs)
{
    FOR_EACH(iter, segs){
        OmActions::ValidateSegment(SegmentDataWrapper(*iter), om::SET_VALID, true);
    }
}

void OmActions::JoinSegments(const OmID segmentationID,
                             const OmSegIDsSet& ids)
{
    SegmentationDataWrapper sdw(segmentationID);

    OmSegPtrSet oldValidSet;

    if(sdw.SegmentCache()->AreAnySegmentsInValidList(ids)){
        // valid segment are present in list
        oldValidSet = setNotValid(sdw.SegmentCache(), ids);
    }

    (new OmSegmentJoinAction(segmentationID, ids))->Run();

    if(oldValidSet.size()){
        setAsValid(oldValidSet);
    }
}

void OmActions::FindAndSplitSegments(const SegmentDataWrapper& curSDW,
                                     OmViewGroupState* vgs,
                                     const DataCoord curClickPt)
{
    const boost::optional<DataCoord>& prevClickPt = vgs->Splitting()->Coord();

    if(prevClickPt){

        const SegmentDataWrapper& prevSDW = vgs->Splitting()->Segment();

        OmSegment* seg1 = prevSDW.getSegment();
        OmSegment* seg2 = curSDW.getSegment();

        if(!seg1 || !seg2) {
            return;
        }

        runIfSplittable(seg1, seg2, *prevClickPt, curClickPt);

        vgs->Splitting()->SetSplitMode(false);

    } else { // set segment to be split later...
        if(curSDW.IsSegmentValid()){
            vgs->Splitting()->SetSplitMode(curSDW, curClickPt);
        }
    }
}

void OmActions::FindAndCutSegments(const SegmentDataWrapper& sdw,
                                   OmViewGroupState* vgs)
{
    doFindAndCutSegment(sdw, vgs);
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

void OmActions::doFindAndCutSegment(const SegmentDataWrapper& sdw,
                                    OmViewGroupState* vgs)
{
    OmSegment* seg1 = sdw.getSegment();
    OmSegment* seg2 = seg1->getParent();

    runIfSplittable(seg1, seg2, DataCoord(), DataCoord());

    vgs->SetCutMode(false);
}

void OmActions::runIfSplittable(OmSegment* seg1, OmSegment* seg2,
                                const DataCoord& coord1, const DataCoord& coord2)
{
    if(seg1 == seg2) {
        (new OmSegmentSplitAction(SegmentDataWrapper(seg1), coord1, coord2))->Run();
        return;
    }

    SegmentationDataWrapper sdw(seg1);

    OmSegmentEdge edge =
        OmFindCommonEdge::FindClosestCommonEdge(sdw.SegmentCache(), seg1, seg2);

    if(!edge.isValid()){
        printf("edge was not splittable\n");
        return;
    }

    (new OmSegmentSplitAction(sdw, edge))->Run();
}
