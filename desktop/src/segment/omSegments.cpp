#include "common/common.h"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "volume/omSegmentation.h"
#include "utility/segmentationDataWrapper.hpp"
#include "volume/metadataManager.h"

using namespace om;
using namespace om::segment;

OmSegments::OmSegments(OmSegmentation* vol)
    : meta_(vol->Metadata()),
      impl_(std::make_unique<OmSegmentsImpl>(vol->SegData(), vol->SegListData(),
                                             vol->MST(), vol->Metadata(),
                                             vol->ValidGroupNum(),
                                             SegmentationDataWrapper(vol))) {}

OmSegments::~OmSegments() {}

void OmSegments::refreshTree() {
  zi::guard g(mutex_);
  impl_->refreshTree();
}

OmSegment* OmSegments::AddSegment() {
  zi::guard g(mutex_);
  return impl_->AddSegment();
}

OmSegment* OmSegments::AddSegment(common::SegID value) {
  zi::guard g(mutex_);
  return impl_->AddSegment(value);
}

OmSegment* OmSegments::GetOrAddSegment(const common::SegID val) {
  zi::guard g(mutex_);
  return impl_->GetOrAddSegment(val);
}

bool OmSegments::IsSegmentValid(common::SegID seg) {
  // FIXME: double check store locking
  zi::guard g(mutex_);
  return impl_->IsSegmentValid(seg);
}

OmSegment* OmSegments::GetSegment(const common::SegID value) const {
  // FIXME: double check store locking
  zi::guard g(mutex_);
  return impl_->GetSegment(value);
}

int OmSegments::GetNumSegments() {
  // locked internally
  return meta_.numSegments();
}

void OmSegments::SetNumSegments(int num) {
  // locked internally
  meta_.set_numSegments(num);
}

void OmSegments::SetName(common::SegID segID, std::string name) {
  zi::guard g(mutex_);
  impl_->SetName(segID, name);
}

std::string OmSegments::GetName(common::SegID segID) {
  zi::guard g(mutex_);
  return impl_->GetName(segID);
}

void OmSegments::SetNote(common::SegID segID, std::string note) {
  zi::guard g(mutex_);
  impl_->SetNote(segID, note);
}

std::string OmSegments::GetNote(common::SegID segID) {
  zi::guard g(mutex_);
  return impl_->GetNote(segID);
}

OmSegment* OmSegments::FindRoot(OmSegment* segment) {
  // FIXME: double check store locking
  zi::guard g(mutex_);
  return impl_->FindRoot(segment->value());
}

OmSegment* OmSegments::FindRoot(const common::SegID segID) {
  // FIXME: no longer locked internally
  zi::guard g(mutex_);
  return impl_->FindRoot(segID);
}

common::SegID OmSegments::FindRootID(const common::SegID segID) {
  // FIXME: no longer locked internally
  zi::guard g(mutex_);
  return impl_->FindRootID(segID);
}

common::SegID OmSegments::FindRootID(OmSegment* segment) {
  // FIXME: no longer locked internally
  zi::guard g(mutex_);
  return impl_->FindRootID(segment->value());
}

common::SegIDSet OmSegments::JoinTheseSegments(
    const common::SegIDSet& segmentList) {
  zi::guard g(mutex_);
  return impl_->JoinTheseSegments(segmentList);
}

common::SegIDSet OmSegments::UnJoinTheseSegments(
    const common::SegIDSet& segmentList) {
  zi::guard g(mutex_);
  return impl_->UnJoinTheseSegments(segmentList);
}

uint32_t OmSegments::maxValue() {
  // locked internally
  return meta_.maxSegments();
}

std::pair<bool, om::segment::UserEdge> OmSegments::JoinEdge(
    const om::segment::UserEdge& e) {
  zi::guard g(mutex_);
  return impl_->JoinFromUserAction(e);
}

om::segment::UserEdge OmSegments::SplitEdge(const om::segment::UserEdge& e) {
  zi::guard g(mutex_);
  return impl_->SplitEdgeUserAction(e);
}

void OmSegments::Flush() {
  zi::guard g(mutex_);
  return impl_->Flush();
}

bool OmSegments::AreAnySegmentsInValidList(const common::SegIDSet& ids) {
  zi::guard g(mutex_);
  return impl_->AreAnySegmentsInValidList(ids);
}

om::segment::Children& OmSegments::Children() const {
  // TODO: needs locking!
  return impl_->Children();
}

boost::optional<std::string> OmSegments::IsEdgeSplittable(
    const om::segment::UserEdge& e) {
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

std::vector<om::segment::UserEdge> OmSegments::CutSegment(OmSegment* seg) {
  zi::guard g(mutex_);
  return impl_->CutSegment(seg);
}

bool OmSegments::JoinEdges(const std::vector<om::segment::UserEdge>& edges) {
  zi::guard g(mutex_);
  return impl_->JoinEdges(edges);
}

std::vector<om::segment::UserEdge> OmSegments::Shatter(OmSegment* seg) {
  zi::guard g(mutex_);
  return impl_->Shatter(seg);
}

om::segment::Selection& OmSegments::Selection() const {
  // locked internally
  return impl_->Selection();
}

OmSegmentLists& OmSegments::SegmentLists() const {
  // locked internally?
  return impl_->SegmentLists();
}
