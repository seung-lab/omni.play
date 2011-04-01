#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omCutting.hpp"
#include "segment/omFindCommonEdge.hpp"
#include "actions/omSelectSegmentParams.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegmentUtils.hpp"

#include "actions/details/omActionImpls.hpp"
#include "actions/details/omSegmentSplitAction.h"
#include "actions/details/omSegmentUncertainAction.h"
#include "actions/details/omSegmentValidateAction.h"
#include "actions/details/omVoxelSetValueAction.h"

#include "actions/omActionsImpl.h"
#include "utility/dataWrappers.h"
#include "system/omLocalPreferences.hpp"
#include "viewGroup/omSplitting.hpp"

#include "utility/omThreadPool.hpp"

OmActionsImpl::OmActionsImpl()
    : threadPool_(new OmThreadPool())
{
    threadPool_->start(1);
}

OmActionsImpl::~OmActionsImpl(){
    threadPool_->join();
}

// project-related
void OmActionsImpl::Save()
{
    (new OmProjectSaveAction())->RunNow();
}

void OmActionsImpl::Close()
{
    (new OmProjectCloseAction())->RunNow();
}

// MST-related
void OmActionsImpl::ChangeMSTthreshold(const SegmentationDataWrapper sdw,
                                       const double threshold)
{
    (new OmSegmentationThresholdChangeAction(sdw, threshold))->Run();
}

//painting-related
void OmActionsImpl::SetVoxel(const OmID segmentationID,
                         const DataCoord voxel,
                         const OmSegID segmentID)
{
    (new OmVoxelSetValueAction(segmentationID,
                               voxel,
                               segmentID))->Run();
}

void OmActionsImpl::SetVoxels(const OmID segmentationID,
                          const std::set<DataCoord> voxels,
                          const OmSegID segmentID)
{
    (new OmVoxelSetValueAction(segmentationID,
                               voxels,
                               segmentID))->Run();
}

// segment-related
void OmActionsImpl::ValidateSegment(const SegmentDataWrapper sdw,
                                const om::SetValid valid, const bool dontCenter)
{
    const OmSegID nextSegmentIDtoJumpTo =
        OmSegmentUtils::GetNextSegIDinWorkingList(sdw);

    OmSegmentValidateAction::Validate(sdw, valid);

    OmSegments* segments = sdw.Segments();

    const bool segmentGettingSetAsValid = om::SET_VALID == valid;
    const bool shouldJump =
        OmLocalPreferences::GetShouldJumpToNextSegmentAfterValidate();
    const bool justOneSegmentSelected =
        (1 == segments->GetSelectedSegmentIds().size());

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

void OmActionsImpl::ValidateSelectedSegments(const SegmentationDataWrapper sdw,
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

void OmActionsImpl::UncertainSegment(const SegmentDataWrapper sdw,
                                 const bool uncertain)
{
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

void OmActionsImpl::UncertainSegmentation(const SegmentationDataWrapper sdw,
                                          const bool uncertain)
{
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

void OmActionsImpl::setUncertain(const SegmentDataWrapper& sdw,
                                 const bool uncertain)
{
    OmSegIDsSet set;
    set.insert(sdw.FindRootID());

    boost::shared_ptr<std::set<OmSegment*> > children =
        OmSegmentUtils::GetAllChildrenSegments(sdw.Segments(), set);

    (new OmSegmentUncertainAction(sdw.MakeSegmentationDataWrapper(),
                                  children, uncertain))->Run();
}

void OmActionsImpl::setUncertain(const SegmentationDataWrapper& sdw,
                                 const bool uncertain)
{
    OmSegments* segments = sdw.Segments();
    boost::shared_ptr<std::set<OmSegment*> > children =
        OmSegmentUtils::GetAllChildrenSegments(segments,
                                               segments->GetSelectedSegmentIds());

    (new OmSegmentUncertainAction(sdw, children, uncertain))->Run();
}

OmSegPtrSet OmActionsImpl::setNotValid(OmSegments* cache, const OmSegIDsSet& ids)
{
    OmSegPtrSet ret;

    FOR_EACH(iter, ids){
        OmSegment* seg = cache->GetSegment(*iter);
        if(!seg){
            continue;
        }
        if(seg->IsValidListType()){
            OmActionsImpl::ValidateSegment(SegmentDataWrapper(seg), om::SET_NOT_VALID);
            ret.insert(seg);
        }
    }
    return ret;
}

void OmActionsImpl::setAsValid(const OmSegPtrSet& segs)
{
    FOR_EACH(iter, segs){
        OmActionsImpl::ValidateSegment(SegmentDataWrapper(*iter), om::SET_VALID, true);
    }
}

void OmActionsImpl::JoinSegmentsID(const OmID segmentationID)
{
    SegmentationDataWrapper sdw(segmentationID);
    if(!sdw.IsSegmentationValid()){
        return;
    }

    OmSegmentation & seg = sdw.GetSegmentation();
    const OmSegIDsSet ids = seg.Segments()->GetSelectedSegmentIds();
    doJoinSegments(sdw, ids);
}

void OmActionsImpl::JoinSegmentsWrapper(const SegmentationDataWrapper sdw)
{
    OmSegmentation & seg = sdw.GetSegmentation();
    const OmSegIDsSet ids = seg.Segments()->GetSelectedSegmentIds();
    doJoinSegments(sdw, ids);
}

void OmActionsImpl::JoinSegmentsSet(const OmID segmentationID,
                                    const OmSegIDsSet ids)
{
    SegmentationDataWrapper sdw(segmentationID);
    if(!sdw.IsSegmentationValid()){
        return;
    }

    doJoinSegments(sdw, ids);
}

void OmActionsImpl::doJoinSegments(const SegmentationDataWrapper& sdw,
                                   const OmSegIDsSet& ids)
{
    OmSegPtrSet oldValidSet;

    if(sdw.Segments()->AreAnySegmentsInValidList(ids)){
        // valid segment are present in list
        oldValidSet = setNotValid(sdw.Segments(), ids);
    }

    (new OmSegmentJoinAction(sdw, ids))->Run();

    if(oldValidSet.size()){
        setAsValid(oldValidSet);
    }
}

void OmActionsImpl::FindAndSplitSegments(const SegmentDataWrapper curSDW,
                                         OmViewGroupState* vgs,
                                         const DataCoord curClickPt)
{
    const boost::optional<DataCoord> prevClickPt = vgs->Splitting()->Coord();

    if(prevClickPt)
    {
        const SegmentDataWrapper prevSDW = vgs->Splitting()->Segment();

        OmSegment* seg1 = prevSDW.GetSegment();
        OmSegment* seg2 = curSDW.GetSegment();

        if(!seg1 || !seg2) {
            return;
        }

        vgs->Splitting()->ShowBusy(true);
        vgs->Splitting()->ExitSplitModeFixButton();

        runIfSplittable(seg1, seg2, *prevClickPt, curClickPt);

        vgs->Splitting()->ShowBusy(false);

    } else {
        if(curSDW.IsSegmentValid())
        {
            // set segment to be split later...
            vgs->Splitting()->SetFirstSplitPoint(curSDW, curClickPt);
        }
    }
}

void OmActionsImpl::FindAndCutSegments(const SegmentDataWrapper sdw,
                                   OmViewGroupState* vgs)
{
    doFindAndCutSegment(sdw, vgs);
}

void OmActionsImpl::SelectSegments(boost::shared_ptr<OmSelectSegmentsParams> params){
    (new OmSegmentSelectAction(params))->Run();
}

// group-related
void OmActionsImpl::CreateOrDeleteSegmentGroup(const OmID segmentationID,
                                           const OmSegIDsSet selectedSegmentIDs,
                                           const OmGroupName name,
                                           const bool create)
{
    (new OmSegmentGroupAction(segmentationID,
                              selectedSegmentIDs,
                              name,
                              create))->Run();
}

void OmActionsImpl::doFindAndCutSegment(const SegmentDataWrapper& sdw,
                                        OmViewGroupState* vgs)
{
    OmSegment* seg1 = sdw.GetSegment();
    OmSegment* seg2 = seg1->getParent();

    runIfSplittable(seg1, seg2, DataCoord(), DataCoord());

    vgs->Cutting()->ExitCutModeFixButton();
}

void OmActionsImpl::runIfSplittable(OmSegment* seg1, OmSegment* seg2,
                                    const DataCoord& coord1, const DataCoord& coord2)
{
    if(seg1 == seg2) {
        (new OmSegmentSplitAction(SegmentDataWrapper(seg1), coord1, coord2))->Run();
        return;
    }

    SegmentationDataWrapper sdw(seg1);

    OmSegmentEdge edge =
        OmFindCommonEdge::FindClosestCommonEdge(sdw.Segments(), seg1, seg2);

    if(!edge.isValid()){
        printf("edge was not splittable\n");
        return;
    }

    (new OmSegmentSplitAction(sdw, edge))->Run();
}
