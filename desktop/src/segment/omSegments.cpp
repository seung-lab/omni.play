#include "datalayer/omDataPath.h"
#include "segment/lowLevel/omEnabledSegments.hpp"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/lowLevel/store/omSegmentStore.hpp"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "volume/omSegmentation.h"
#include "utility/dataWrappers.h"

OmSegments::OmSegments(OmSegmentation* segmentation)
    : segmentation_(segmentation)
    , store_(new OmSegmentsStore(segmentation))
    , impl_(new OmSegmentsImpl(segmentation, store_.get()))
{}

OmSegments::~OmSegments()
{}

OmSegID OmSegments::GetSegmentationID(){
    return segmentation_->GetID();
}

void OmSegments::refreshTree()
{
    zi::guard g(mutex_);
    impl_->refreshTree();
}

uint32_t OmSegments::getPageSize()
{
    // locked internally
    return store_->PageSize();
}

OmSegment* OmSegments::AddSegment()
{
    zi::guard g(mutex_);
    return impl_->AddSegment();
}

OmSegment* OmSegments::AddSegment(OmSegID value)
{
    zi::guard g(mutex_);
    return impl_->AddSegment(value);
}

OmSegment* OmSegments::GetOrAddSegment(const OmSegID val)
{
    zi::guard g(mutex_);
    return impl_->GetOrAddSegment(val);
}

bool OmSegments::IsSegmentValid(OmSegID seg)
{
    // locked internally
    return store_->IsSegmentValid(seg);
}

OmSegment* OmSegments::GetSegment(const OmSegID value)
{
    // locked internally
    return store_->GetSegment(value);
}

OmSegment* OmSegments::GetSegmentUnsafe(const OmSegID value)
{
    // locked internally
    return store_->GetSegmentUnsafe(value);
}

OmSegID OmSegments::GetNumSegments()
{
    zi::guard g(mutex_);
    return impl_->GetNumSegments();
}

uint32_t OmSegments::GetNumTopSegments()
{
    // locked internally

    if(segmentation_){
        return segmentation_->SegmentLists()->GetNumTopLevelSegs();
    }

    return 0;
}

bool OmSegments::AreSegmentsSelected()
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().AreSegmentsSelected();
}

uint32_t OmSegments::NumberOfSelectedSegments()
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().NumberOfSelectedSegments();
}

const OmSegIDsSet OmSegments::GetSelectedSegmentIDs()
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().GetSelectedSegmentIDs();
}

OmSegIDsSet OmSegments::GetEnabledSegmentIDs()
{
    zi::guard g(mutex_);
    return impl_->EnabledSegments().GetEnabledSegmentIDs();
}

bool OmSegments::isSegmentEnabled(OmSegID segID)
{
    zi::guard g(mutex_);
    return impl_->EnabledSegments().IsEnabled(segID);
}

bool OmSegments::IsSegmentSelected(OmSegID segID)
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().isSegmentSelected(segID);
}

bool OmSegments::IsSegmentSelected(OmSegment* seg)
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().isSegmentSelected(seg->value());
}

void OmSegments::setSegmentEnabled(OmSegID segID, bool isEnabled)
{
    zi::guard g(mutex_);
    impl_->EnabledSegments().SetEnabled(segID, isEnabled);
}

void OmSegments::setSegmentSelected(OmSegID segID,
                                         const bool isSelected,
                                         const bool addToRecentList)
{
    zi::guard g(mutex_);
    impl_->SegmentSelection().setSegmentSelected(segID, isSelected, addToRecentList);
}

void OmSegments::setSegmentName(OmSegID segID, QString name)
{
    zi::guard g(mutex_);
    impl_->setSegmentName(segID, name);
}

QString OmSegments::getSegmentName(OmSegID segID)
{
    zi::guard g(mutex_);
    return impl_->getSegmentName(segID);
}

void OmSegments::setSegmentNote(OmSegID segID, QString note)
{
    zi::guard g(mutex_);
    impl_->setSegmentNote(segID, note);
}

QString OmSegments::getSegmentNote(OmSegID segID)
{
    zi::guard g(mutex_);
    return impl_->getSegmentNote(segID);
}

OmSegment* OmSegments::findRoot(OmSegment* segment)
{
    // locked internally
    return store_->GetSegment(store_->Root(segment->value()));
}

OmSegment* OmSegments::findRoot(const OmSegID segID)
{
    // locked internally
    return store_->GetSegment(store_->Root(segID));
}

OmSegID OmSegments::findRootID(const OmSegID segID)
{
    // locked internally

    if(!segID){
        return 0;
    }

    return store_->Root(segID);
}

OmSegID OmSegments::findRootID(OmSegment* segment)
{
    // locked internally
    return findRootID(segment->value());
}

OmSegID OmSegments::findRootIDnoCache(const OmSegID segID)
{
    zi::guard g(mutex_);
    return impl_->FindRootID(segID);
}

OmSegIDsSet OmSegments::JoinTheseSegments(const OmSegIDsSet & segmentList)
{
    zi::guard g(mutex_);
    return impl_->JoinTheseSegments(segmentList);
}

OmSegIDsSet OmSegments::UnJoinTheseSegments(const OmSegIDsSet & segmentList)
{
    zi::guard g(mutex_);
    return impl_->UnJoinTheseSegments(segmentList);
}

uint32_t OmSegments::getMaxValue()
{
    // self-locking integer
    return impl_->getMaxValue();
}

void OmSegments::UpdateSegmentSelection(const OmSegIDsSet & idsToSelect,
                                             const bool addToRecentList)
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().UpdateSegmentSelection(idsToSelect, addToRecentList);
}

void OmSegments::AddToSegmentSelection(const OmSegIDsSet& idsToSelect)
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().AddToSegmentSelection(idsToSelect);
}

void OmSegments::RemoveFromSegmentSelection(const OmSegIDsSet& idsToSelect)
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().RemoveFromSegmentSelection(idsToSelect);
}

std::pair<bool, OmSegmentEdge> OmSegments::JoinEdge(const OmSegmentEdge& e)
{
    zi::guard g(mutex_);
    return impl_->JoinFromUserAction(e);
}

OmSegmentEdge OmSegments::SplitEdge(const OmSegmentEdge & e)
{
    zi::guard g(mutex_);
    return impl_->SplitEdgeUserAction(e);
}

bool OmSegments::AreSegmentsEnabled()
{
    zi::guard g(mutex_);
    return impl_->EnabledSegments().AnyEnabled();
}

void OmSegments::StartCaches()
{
    zi::guard g(mutex_);
    store_->StartCaches();
}

void OmSegments::Flush()
{
    zi::guard g(mutex_);
    return impl_->Flush();
}

bool OmSegments::AreAnySegmentsInValidList(const OmSegIDsSet& ids)
{
    zi::guard g(mutex_);
    return impl_->AreAnySegmentsInValidList(ids);
}

uint64_t OmSegments::MSTfreshness() const
{
    // locked number
    return impl_->MSTfreshness();
}

OmSegmentChildren* OmSegments::Children()
{
    // TODO: needs locking!
    return impl_->Children();
}

boost::optional<std::string> OmSegments::IsEdgeSplittable(const OmSegmentEdge& e)
{
    zi::guard g(mutex_);
    return impl_->IsEdgeSplittable(e);
}

boost::optional<std::string> OmSegments::IsSegmentSplittable(OmSegment* child)
{
    zi::guard g(mutex_);
    return impl_->IsSegmentSplittable(child);
}

boost::optional<std::string> OmSegments::IsSegmentCuttable(OmSegment* seg)
{
    zi::guard g(mutex_);
    return impl_->IsSegmentCuttable(seg);
}

std::vector<OmSegmentEdge> OmSegments::CutSegment(OmSegment* seg)
{
    zi::guard g(mutex_);
    return impl_->CutSegment(seg);
}

bool OmSegments::JoinEdges(const std::vector<OmSegmentEdge>& edges)
{
    zi::guard g(mutex_);
    return impl_->JoinEdges(edges);
}

void OmSegments::Grow_LocalSizeThreshold(OmMST* MST, OmSegmentSelector* sel, SegmentDataWrapper& sdw)
{
    impl_->Grow_LocalSizeThreshold(MST,sel,sdw.GetSegmentID());
}

void OmSegments::AddSegments_BreadthFirstSearch(OmMST* MST, OmSegmentSelector* sel, SegmentDataWrapper& sdw)
{
    impl_->AddSegments_BreadthFirstSearch(MST,sel,sdw.GetSegmentID());
}

void OmSegments::AddSegments_BFS_DynamicThreshold(OmMST* MST, OmSegmentSelector* sel, SegmentDataWrapper& sdw)
{
    impl_->AddSegments_BFS_DynamicThreshold(MST,sel,sdw.GetSegmentID());
}

void OmSegments::Trim(OmMST* MST, OmSegmentSelector* sel, SegmentDataWrapper& sdw)
{
    impl_->Trim(MST,sel,sdw.GetSegmentID());
}

// void OmSegments::GrowSelection(OmSegmentDataWraper& sdw) {
// 	impl_->GrowSelection(sdw);
// }

// void OmSegments::ShrinkSelection(OmSegmentDataWraper& sdw) {
// 	impl_->ShrinkSelection(sdw);
// }
