#include "actions/details/omActionImpls.hpp"
#include "actions/details/omSegmentSplitAction.h"
#include "actions/details/omSegmentUncertainAction.h"
#include "actions/details/omSegmentValidateAction.h"
#include "actions/details/omVoxelSetValueAction.h"
#include "actions/omActionsImpl.h"
#include "actions/omSelectSegmentParams.hpp"
#include "segment/actions/omCutSegmentRunner.hpp"
#include "segment/actions/omJoinSegmentsRunner.hpp"
#include "segment/actions/omSetSegmentValidRunner.hpp"
#include "segment/omFindCommonEdge.hpp"
#include "segment/omSegmentSelector.h"
#include "system/omLocalPreferences.hpp"
#include "threads/omTaskManager.hpp"
#include "utility/dataWrappers.h"
#include "viewGroup/omSplitting.hpp"
#include "viewGroup/omViewGroupState.h"

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
    OmSetSegmentValidRunner validator(sdw, valid);
    validator.Validate();
    validator.JumpToNextSegment(dontCenter);
}

void OmActionsImpl::ValidateSelectedSegments(const SegmentationDataWrapper sdw,
                                             const om::SetValid valid)
{
    OmSetSegmentsValidRunner validator(sdw, valid);
    validator.Validate();
    validator.JumpToNextSegment();
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

    om::shared_ptr<std::set<OmSegment*> > children =
        OmSegmentUtils::GetAllChildrenSegments(sdw.Segments(), set);

    (new OmSegmentUncertainAction(sdw.MakeSegmentationDataWrapper(),
                                  children, uncertain))->Run();
}

void OmActionsImpl::setUncertain(const SegmentationDataWrapper& sdw,
                                 const bool uncertain)
{
    OmSegments* segments = sdw.Segments();

    om::shared_ptr<std::set<OmSegment*> > children =
        OmSegmentUtils::GetAllChildrenSegments(segments,
                                               segments->GetSelectedSegmentIDs());

    (new OmSegmentUncertainAction(sdw, children, uncertain))->Run();
}

void OmActionsImpl::JoinSegmentsWrapper(const SegmentationDataWrapper sdw)
{
    OmJoinSegmentsRunner joiner(sdw);
    joiner.Join();
}

void OmActionsImpl::JoinSegmentsSet(const SegmentationDataWrapper sdw,
                                    const OmSegIDsSet ids)
{
    OmJoinSegmentsRunner joiner(sdw, ids);
    joiner.Join();
}

void OmActionsImpl::FindAndSplitSegments(OmSegment* seg1, OmSegment* seg2)
{
    if(seg1 == seg2)
    {
        std::cout << "can't split--same segment\n";
        return;
    }

    SegmentationDataWrapper sdw(seg1);

    OmSegmentEdge edge =
        OmFindCommonEdge::FindClosestCommonEdge(sdw.Segments(), seg1, seg2);

    if(!edge.isValid())
    {
        printf("edge was not splittable\n");
        return;
    }

    (new OmSegmentSplitAction(sdw, edge))->Run();
}

void OmActionsImpl::CutSegment(const SegmentDataWrapper sdw)
{
    OmCutSegmentRunner::CutSegmentFromParent(sdw);
}

void OmActionsImpl::SelectSegments(om::shared_ptr<OmSelectSegmentsParams> params){
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
