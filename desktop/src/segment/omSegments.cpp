#include "datalayer/omDataPath.h"
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
    : segmentation_(segmentation),
      store_(new OmSegmentsStore(segmentation)),
      impl_(new OmSegmentsImpl(segmentation, store_.get())) {}

OmSegments::~OmSegments() {}

om::common::SegID OmSegments::GetSegmentationID() {
  return segmentation_->GetID();
}

void OmSegments::refreshTree() {
  zi::guard g(mutex_);
  impl_->refreshTree();
}

uint32_t OmSegments::getPageSize() {
  // locked internally
  return store_->PageSize();
}

OmSegment* OmSegments::AddSegment() {
  zi::guard g(mutex_);
  return impl_->AddSegment();
}

OmSegment* OmSegments::AddSegment(om::common::SegID value) {
  zi::guard g(mutex_);
  return impl_->AddSegment(value);
}

OmSegment* OmSegments::GetOrAddSegment(const om::common::SegID val) {
  zi::guard g(mutex_);
  return impl_->GetOrAddSegment(val);
}

bool OmSegments::IsSegmentValid(om::common::SegID seg) {
  // locked internally
  return store_->IsSegmentValid(seg);
}

OmSegment* OmSegments::GetSegment(const om::common::SegID value) {
  // locked internally
  return store_->GetSegment(value);
}

OmSegment* OmSegments::GetSegmentUnsafe(const om::common::SegID value) {
  // locked internally
  return store_->GetSegmentUnsafe(value);
}

om::common::SegID OmSegments::GetNumSegments() {
  zi::guard g(mutex_);
  return impl_->GetNumSegments();
}

uint32_t OmSegments::GetNumTopSegments() {
  // locked internally

  if (segmentation_) {
    return segmentation_->SegmentLists()->GetNumTopLevelSegs();
  }

  return 0;
}

bool OmSegments::AreSegmentsSelected() {
  zi::guard g(mutex_);
  return impl_->SegmentSelection().AreSegmentsSelected();
}

uint32_t OmSegments::NumberOfSelectedSegments() {
  zi::guard g(mutex_);
  return impl_->SegmentSelection().NumberOfSelectedSegments();
}

const om::common::SegIDSet OmSegments::GetSelectedSegmentIDs() {
  zi::guard g(mutex_);
  return impl_->SegmentSelection().GetSelectedSegmentIDs();
}

om::common::SegIDSet OmSegments::GetEnabledSegmentIDs() {
  zi::guard g(mutex_);
  return impl_->EnabledSegments().GetEnabledSegmentIDs();
}

bool OmSegments::isSegmentEnabled(om::common::SegID segID) {
  zi::guard g(mutex_);
  return impl_->EnabledSegments().IsEnabled(segID);
}

bool OmSegments::IsSegmentSelected(om::common::SegID segID) {
  zi::guard g(mutex_);
  return impl_->SegmentSelection().isSegmentSelected(segID);
}

bool OmSegments::IsSegmentSelected(OmSegment* seg) {
  zi::guard g(mutex_);
  return impl_->SegmentSelection().isSegmentSelected(seg->value());
}

void OmSegments::setSegmentEnabled(om::common::SegID segID, bool isEnabled) {
  zi::guard g(mutex_);
  impl_->EnabledSegments().SetEnabled(segID, isEnabled);
}

void OmSegments::setSegmentSelected(om::common::SegID segID,
                                    const bool isSelected,
                                    const bool addToRecentList) {
  zi::guard g(mutex_);
  impl_->SegmentSelection()
      .setSegmentSelected(segID, isSelected, addToRecentList);
}

void OmSegments::setSegmentName(om::common::SegID segID, QString name) {
  zi::guard g(mutex_);
  impl_->setSegmentName(segID, name);
}

QString OmSegments::getSegmentName(om::common::SegID segID) {
  zi::guard g(mutex_);
  return impl_->getSegmentName(segID);
}

void OmSegments::setSegmentNote(om::common::SegID segID, QString note) {
  zi::guard g(mutex_);
  impl_->setSegmentNote(segID, note);
}

QString OmSegments::getSegmentNote(om::common::SegID segID) {
  zi::guard g(mutex_);
  return impl_->getSegmentNote(segID);
}

OmSegment* OmSegments::findRoot(OmSegment* segment) {
  // locked internally
  return store_->GetSegment(store_->Root(segment->value()));
}

OmSegment* OmSegments::findRoot(const om::common::SegID segID) {
  // locked internally
  return store_->GetSegment(store_->Root(segID));
}

om::common::SegID OmSegments::findRootID(const om::common::SegID segID) {
  // locked internally

  if (!segID) {
    return 0;
  }

  return store_->Root(segID);
}

om::common::SegID OmSegments::findRootID(OmSegment* segment) {
  // locked internally
  return findRootID(segment->value());
}

om::common::SegID OmSegments::findRootIDnoCache(const om::common::SegID segID) {
  zi::guard g(mutex_);
  return impl_->FindRootID(segID);
}

om::common::SegIDSet OmSegments::JoinTheseSegments(
    const om::common::SegIDSet& segmentList) {
  zi::guard g(mutex_);
  return impl_->JoinTheseSegments(segmentList);
}

om::common::SegIDSet OmSegments::UnJoinTheseSegments(
    const om::common::SegIDSet& segmentList) {
  zi::guard g(mutex_);
  return impl_->UnJoinTheseSegments(segmentList);
}

uint32_t OmSegments::getMaxValue() {
  // self-locking integer
  return impl_->getMaxValue();
}

void OmSegments::UpdateSegmentSelection(const om::common::SegIDSet& idsToSelect,
                                        const bool addToRecentList) {
  zi::guard g(mutex_);
  return impl_->SegmentSelection()
      .UpdateSegmentSelection(idsToSelect, addToRecentList);
}

void OmSegments::AddToSegmentSelection(
    const om::common::SegIDSet& idsToSelect) {
  zi::guard g(mutex_);
  return impl_->SegmentSelection().AddToSegmentSelection(idsToSelect);
}

void OmSegments::RemoveFromSegmentSelection(
    const om::common::SegIDSet& idsToSelect) {
  zi::guard g(mutex_);
  return impl_->SegmentSelection().RemoveFromSegmentSelection(idsToSelect);
}

std::pair<bool, OmSegmentEdge> OmSegments::JoinEdge(const OmSegmentEdge& e) {
  zi::guard g(mutex_);
  return impl_->JoinFromUserAction(e);
}

OmSegmentEdge OmSegments::SplitEdge(const OmSegmentEdge& e) {
  zi::guard g(mutex_);
  return impl_->SplitEdgeUserAction(e);
}

bool OmSegments::AreSegmentsEnabled() {
  zi::guard g(mutex_);
  return impl_->EnabledSegments().AnyEnabled();
}

void OmSegments::StartCaches() {
  zi::guard g(mutex_);
  store_->StartCaches();
}

void OmSegments::Flush() {
  zi::guard g(mutex_);
  return impl_->Flush();
}

bool OmSegments::AreAnySegmentsInValidList(const om::common::SegIDSet& ids) {
  zi::guard g(mutex_);
  return impl_->AreAnySegmentsInValidList(ids);
}

uint64_t OmSegments::MSTfreshness() const {
  // locked number
  return impl_->MSTfreshness();
}

OmSegmentChildren* OmSegments::Children() {
  // TODO: needs locking!
  return impl_->Children();
}

boost::optional<std::string> OmSegments::IsEdgeSplittable(
    const OmSegmentEdge& e) {
  zi::guard g(mutex_);
  return impl_->IsEdgeSplittable(e);
}

boost::optional<std::string> OmSegments::IsSegmentSplittable(OmSegment* child) {
  zi::guard g(mutex_);
  return impl_->IsSegmentSplittable(child);
}

boost::optional<std::string> OmSegments::IsSegmentCuttable(OmSegment* seg) {
  zi::guard g(mutex_);
  return impl_->IsSegmentCuttable(seg);
}

std::vector<OmSegmentEdge> OmSegments::CutSegment(OmSegment* seg) {
  zi::guard g(mutex_);
  return impl_->CutSegment(seg);
}

bool OmSegments::JoinEdges(const std::vector<OmSegmentEdge>& edges) {
  zi::guard g(mutex_);
  return impl_->JoinEdges(edges);
}

std::vector<OmSegmentEdge> OmSegments::Shatter(OmSegment* seg) {
  zi::guard g(mutex_);
  return impl_->Shatter(seg);
}
