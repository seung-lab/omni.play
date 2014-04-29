#include "segment/lists/omSegmentListByMRU.hpp"
#include "segment/lists/omSegmentListForGUI.hpp"
#include "segment/lists/omSegmentListGlobal.hpp"
#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "zi/utility.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/lists/omSegmentListsImpl.hpp"

OmSegmentLists::OmSegmentLists(om::volume::MetadataManager& meta,
                               om::segment::Store& store,
                               SegmentationDataWrapper sdw)
    : globalList_(new OmSegmentListGlobal()),
      working_(new OmSegmentListForGUI(om::common::SegListType::WORKING)),
      uncertain_(new OmSegmentListForGUI(om::common::SegListType::UNCERTAIN)),
      valid_(new OmSegmentListForGUI(om::common::SegListType::VALID)),
      recent_(new OmSegmentListByMRU(*this)) {
  segmentListsLL_ =
      std::make_unique<OmSegmentListsImpl>(*this, meta, store, sdw);
}

OmSegmentLists::~OmSegmentLists() {}

void OmSegmentLists::Resize(size_t size) { segmentListsLL_->Resize(size); }

void OmSegmentLists::TouchRecent(OmSegment* seg) {
  // recent is internally locked
  recent_->Touch(seg);
}

size_t OmSegmentLists::SizeRecent() {
  // recent is internally locked
  return recent_->Size();
}

std::shared_ptr<GUIPageOfSegments> OmSegmentLists::GetSegmentGUIPageRecent(
    const GUIPageRequest& request) {
  // recent is internally locked
  return recent_->GetSegmentGUIPage(request);
}

void OmSegmentLists::Swap(std::shared_ptr<OmSegmentListForGUI>& list) {
  zi::rwmutex::write_guard g(lock_);

  switch (list->Type()) {
    case om::common::SegListType::VALID:
      valid_ = list;
      break;
    case om::common::SegListType::WORKING:
      working_ = list;
      break;
    case om::common::SegListType::UNCERTAIN:
      uncertain_ = list;
      break;
    default:
      throw om::ArgException("unknown type");
  }
}

void OmSegmentLists::Swap(std::shared_ptr<OmSegmentListGlobal>& globalList) {
  zi::rwmutex::write_guard g(lock_);
  globalList_ = globalList;
}

size_t OmSegmentLists::Size(const om::common::SegListType type) {
  zi::rwmutex::read_guard g(lock_);
  return get(type)->Size();
}

uint64_t OmSegmentLists::GetNumTopLevelSegs() {
  zi::rwmutex::read_guard g(lock_);
  return working_->Size() + valid_->Size() + uncertain_->Size();
}

int64_t OmSegmentLists::TotalNumVoxels() {
  zi::rwmutex::read_guard g(lock_);

  return working_->TotalNumVoxels() + valid_->TotalNumVoxels() +
         uncertain_->TotalNumVoxels();
}

int64_t OmSegmentLists::NumVoxels(const om::common::SegListType type) {
  zi::rwmutex::read_guard g(lock_);
  return get(type)->TotalNumVoxels();
}

std::shared_ptr<GUIPageOfSegments> OmSegmentLists::GetSegmentGUIPage(
    const om::common::SegListType type, const GUIPageRequest& request) {
  zi::rwmutex::read_guard g(lock_);
  return get(type)->GetSegmentGUIPage(request);
}

int64_t OmSegmentLists::GetSizeWithChildren(const om::common::SegID segID) {
  zi::rwmutex::read_guard g(lock_);
  return globalList_->GetSizeWithChildren(segID);
}

int64_t OmSegmentLists::GetSizeWithChildren(OmSegment* seg) {
  zi::rwmutex::read_guard g(lock_);
  return globalList_->GetSizeWithChildren(seg);
}

om::common::SegID OmSegmentLists::GetNextSegIDinWorkingList(
    const SegmentDataWrapper& sdw) {
  zi::rwmutex::read_guard g(lock_);

  if (!sdw.IsSegmentValid()) {
    return 0;
  }

  boost::optional<SegInfo> segInfo = working_->GetNextSegID(sdw.GetSegmentID());
  if (!segInfo) {
    return 0;
  }
  return segInfo->segID;
}

// choose largest segment
om::common::SegID OmSegmentLists::GetNextSegIDinWorkingList(
    const SegmentationDataWrapper& sdw) {
  zi::rwmutex::read_guard g(lock_);

  if (!sdw.IsSegmentationValid()) {
    return 0;
  }

  const om::common::SegIDSet selectedIDs = sdw.GetSelectedSegmentIDs();
  std::vector<SegInfo> infos;
  infos.reserve(selectedIDs.size());

  for (auto id : selectedIDs) {
    boost::optional<SegInfo> segInfo = working_->Get(id);
    if (!segInfo) {
      continue;
    }
    infos.push_back(*segInfo);
  }

  if (infos.empty()) {
    return 0;
  }

  // largest segInfo is the first segInfo
  const SegInfo largest =
      *zi::min_element(infos.begin(), infos.end(), om::segLists::CmpSegInfo);

  return largest.segID;
}

void OmSegmentLists::RefreshGUIlists() {
  segmentListsLL_->ForceRefreshGUIlists();
}

int64_t OmSegmentLists::GetNumChildren(const om::common::SegID segID) {
  zi::rwmutex::read_guard g(lock_);
  return globalList_->GetNumChildren(segID);
}

int64_t OmSegmentLists::GetNumChildren(OmSegment* seg) {
  zi::rwmutex::read_guard g(lock_);
  return globalList_->GetNumChildren(seg);
}

void OmSegmentLists::UpdateSizeListsFromJoin(OmSegment* root,
                                             OmSegment* child) {
  segmentListsLL_->UpdateSizeListsFromJoin(root, child);
}
void OmSegmentLists::UpdateSizeListsFromSplit(
    OmSegment* root, OmSegment* child, const SizeAndNumPieces& childInfo) {
  segmentListsLL_->UpdateSizeListsFromSplit(root, child, childInfo);
}
