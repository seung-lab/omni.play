#include "datalayer/dataPaths.h"
#include "segment/segment.h"
#include "segment/segments.h"
#include "segment/segmentsImpl.h"
#include "volume/segmentation.h"

namespace om {
namespace segment {

segments::segments(segmentation* segmentation)
    : segmentation_(segmentation)
    , impl_(new segmentsImpl(segmentation, store_.get()))
{}

segments::~segments()
{}

common::segId segments::GetSegmentationID(){
    return segmentation_->GetID();
}

void segments::refreshTree()
{
    zi::guard g(mutex_);
    impl_->refreshTree();
}

uint32_t segments::getPageSize()
{
    // locked internally
    return store_->PageSize();
}

segment* segments::AddSegment()
{
    zi::guard g(mutex_);
    return impl_->AddSegment();
}

segment* segments::AddSegment(common::segId value)
{
    zi::guard g(mutex_);
    return impl_->AddSegment(value);
}

segment* segments::GetOrAddSegment(const common::segId val)
{
    zi::guard g(mutex_);
    return impl_->GetOrAddSegment(val);
}

bool segments::IsSegmentValid(common::segId seg)
{
    // locked internally
    return store_->IsSegmentValid(seg);
}

segment* segments::GetSegment(const common::segId value)
{
    // locked internally
    return store_->GetSegment(value);
}

segment* segments::GetSegmentUnsafe(const common::segId value)
{
    // locked internally
    return store_->GetSegmentUnsafe(value);
}

common::segId segments::GetNumSegments()
{
    zi::guard g(mutex_);
    return impl_->GetNumSegments();
}

uint32_t segments::GetNumTopSegments()
{
    // locked internally

    if(segmentation_){
        return segmentation_->SegmentLists()->GetNumTopLevelSegs();
    }

    return 0;
}

bool segments::AreSegmentsSelected()
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().AreSegmentsSelected();
}

uint32_t segments::NumberOfSelectedSegments()
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().NumberOfSelectedSegments();
}

const common::segIdSet segments::GetSelectedSegmentIDs()
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().GetSelectedSegmentIDs();
}

common::segIdSet segments::GetEnabledSegmentIDs()
{
    zi::guard g(mutex_);
    return impl_->EnabledSegments().GetEnabledSegmentIDs();
}

bool segments::isSegmentEnabled(common::segId segID)
{
    zi::guard g(mutex_);
    return impl_->EnabledSegments().IsEnabled(segID);
}

bool segments::IsSegmentSelected(common::segId segID)
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().isSegmentSelected(segID);
}

bool segments::IsSegmentSelected(segment* seg)
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().isSegmentSelected(seg->value());
}

void segments::setSegmentEnabled(common::segId segID, bool isEnabled)
{
    zi::guard g(mutex_);
    impl_->EnabledSegments().SetEnabled(segID, isEnabled);
}

void segments::setSegmentSelected(common::segId segID,
                                         const bool isSelected,
                                         const bool addToRecentList)
{
    zi::guard g(mutex_);
    impl_->SegmentSelection().setSegmentSelected(segID, isSelected, addToRecentList);
}

void segments::setSegmentName(common::segId segID, std::string name)
{
    zi::guard g(mutex_);
    impl_->setSegmentName(segID, name);
}

std::string segments::getSegmentName(common::segId segID)
{
    zi::guard g(mutex_);
    return impl_->getSegmentName(segID);
}

void segments::setSegmentNote(common::segId segID, std::string note)
{
    zi::guard g(mutex_);
    impl_->setSegmentNote(segID, note);
}

std::string segments::getSegmentNote(common::segId segID)
{
    zi::guard g(mutex_);
    return impl_->getSegmentNote(segID);
}

segment* segments::findRoot(segment* segment)
{
    // locked internally
    return store_->GetSegment(store_->Root(segment->value()));
}

segment* segments::findRoot(const common::segId segID)
{
    // locked internally
    return store_->GetSegment(store_->Root(segID));
}

common::segId segments::findRootID(const common::segId segID)
{
    // locked internally

    if(!segID){
        return 0;
    }

    return store_->Root(segID);
}

common::segId segments::findRootID(segment* segment)
{
    // locked internally
    return findRootID(segment->value());
}

common::segId segments::findRootIDnoCache(const common::segId segID)
{
    zi::guard g(mutex_);
    return impl_->FindRootID(segID);
}

common::segIdSet segments::JoinTheseSegments(const common::segIdSet & segmentList)
{
    zi::guard g(mutex_);
    return impl_->JoinTheseSegments(segmentList);
}

common::segIdSet segments::UnJoinTheseSegments(const common::segIdSet & segmentList)
{
    zi::guard g(mutex_);
    return impl_->UnJoinTheseSegments(segmentList);
}

uint32_t segments::getMaxValue()
{
    // self-locking integer
    return impl_->getMaxValue();
}

void segments::UpdateSegmentSelection(const common::segIdSet & idsToSelect,
                                             const bool addToRecentList)
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().UpdateSegmentSelection(idsToSelect, addToRecentList);
}

void segments::AddToSegmentSelection(const common::segIdSet& idsToSelect)
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().AddToSegmentSelection(idsToSelect);
}

void segments::RemoveFromSegmentSelection(const common::segIdSet& idsToSelect)
{
    zi::guard g(mutex_);
    return impl_->SegmentSelection().RemoveFromSegmentSelection(idsToSelect);
}

std::pair<bool, segmentEdge> segments::JoinEdge(const segmentEdge& e)
{
    zi::guard g(mutex_);
    return impl_->JoinFromUserAction(e);
}

segmentEdge segments::SplitEdge(const segmentEdge & e)
{
    zi::guard g(mutex_);
    return impl_->SplitEdgeUserAction(e);
}

bool segments::AreSegmentsEnabled()
{
    zi::guard g(mutex_);
    return impl_->EnabledSegments().AnyEnabled();
}

void segments::StartCaches()
{
    zi::guard g(mutex_);
    store_->StartCaches();
}

void segments::Flush()
{
    zi::guard g(mutex_);
    return impl_->Flush();
}

bool segments::AreAnySegmentsInValidList(const common::segIdSet& ids)
{
    zi::guard g(mutex_);
    return impl_->AreAnySegmentsInValidList(ids);
}

uint64_t segments::MSTfreshness() const
{
    // locked number
    return impl_->MSTfreshness();
}

segmentChildren* segments::Children()
{
    // TODO: needs locking!
    return impl_->Children();
}

boost::optional<std::string> segments::IsEdgeSplittable(const segmentEdge& e)
{
    zi::guard g(mutex_);
    return impl_->IsEdgeSplittable(e);
}

boost::optional<std::string> segments::IsSegmentSplittable(segment* child)
{
    zi::guard g(mutex_);
    return impl_->IsSegmentSplittable(child);
}

boost::optional<std::string> segments::IsSegmentCuttable(segment* seg)
{
    zi::guard g(mutex_);
    return impl_->IsSegmentCuttable(seg);
}

std::vector<segmentEdge> segments::CutSegment(segment* seg)
{
    zi::guard g(mutex_);
    return impl_->CutSegment(seg);
}

bool segments::JoinEdges(const std::vector<segmentEdge>& edges)
{
    zi::guard g(mutex_);
    return impl_->JoinEdges(edges);
}

} // namespace segment
} // namespace om
