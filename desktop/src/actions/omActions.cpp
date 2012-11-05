#include "actions/omActions.h"
#include "actions/omActionsImpl.h"
#include "utility/dataWrappers.h"
#include "threads/omTaskManager.hpp"

OmThreadPool& OmActions::pool(){
    return OmProject::Globals().Actions().ThreadPool();
}

// project-related
void OmActions::Save()
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::Save, impl())));
}

void OmActions::Close()
{
    GenerateCloseAction();
    pool().join();
}

void OmActions::GenerateCloseAction()
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::Close, impl())));
}

// MST-related
void OmActions::ChangeMSTthreshold(const SegmentationDataWrapper sdw,
                                   const double threshold)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::ChangeMSTthreshold, impl(),
                     sdw, threshold)));
}

void OmActions::ChangeSizethreshold(const SegmentationDataWrapper sdw,
                                   const double threshold)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::ChangeSizethreshold, impl(),
                     sdw, threshold)));
}

//painting-related
void OmActions::SetVoxel(const OmID segmentationID,
                         const om::coords::Global& voxel,
                         const OmSegID segmentID)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::SetVoxel, impl(),
                     segmentationID, voxel, segmentID)));
}

void OmActions::SetVoxels(const OmID segmentationID,
                          const std::set<om::coords::Global>& voxels,
                          const OmSegID segmentID)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::SetVoxels, impl(),
                     segmentationID, voxels, segmentID)));
}

// segment-related
void OmActions::ValidateSegment(const SegmentDataWrapper& sdw,
                                const om::common::SetValid valid, const bool dontCenter)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::ValidateSegment, impl(),
                     sdw, valid, dontCenter)));
}

void OmActions::ValidateSelectedSegments(const SegmentationDataWrapper& sdw,
                                         const om::common::SetValid valid)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::ValidateSelectedSegments, impl(),
                     sdw, valid)));
}

void OmActions::UncertainSegment(const SegmentDataWrapper& sdw,
                                 const bool uncertain)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::UncertainSegment, impl(),
                     sdw, uncertain)));
}

void OmActions::UncertainSegmentation(const SegmentationDataWrapper& sdw,
                                      const bool uncertain)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::UncertainSegmentation, impl(),
                     sdw, uncertain)));
}

void OmActions::JoinSegments(const SegmentationDataWrapper& sdw)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::JoinSegmentsWrapper, impl(),
                     sdw)));
}

void OmActions::JoinSegments(const SegmentationDataWrapper& sdw,
                             const OmSegIDsSet& ids)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::JoinSegmentsSet, impl(),
                     sdw, ids)));
}

void OmActions::FindAndSplitSegments(OmSegment* seg1, OmSegment* seg2)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::FindAndSplitSegments, impl(),
                     seg1, seg2)));
}

void OmActions::ShatterSegment(OmSegment* seg)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::ShatterSegment, impl(),
                     seg)));
}

void OmActions::CutSegment(const SegmentDataWrapper& sdw)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::CutSegment, impl(),
                     sdw)));
}

void OmActions::SelectSegments(om::shared_ptr<OmSelectSegmentsParams> params)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::SelectSegments, impl(),
                     params)));
}

// group-related
void OmActions::CreateOrDeleteSegmentGroup(const OmID segmentationID,
                                           const OmSegIDsSet& selectedSegmentIDs,
                                           const OmGroupName name,
                                           const bool create)
{
    pool().push_back(
        zi::run_fn(
            zi::bind(&OmActionsImpl::CreateOrDeleteSegmentGroup, impl(),
                     segmentationID, selectedSegmentIDs, name, create)));
}

