#pragma once
#include "precomp.h"

#include "common/enums.hpp"
#include "segment/lists/omSegmentListsTypes.hpp"

class OmSegmentListLowLevel;
class OmSegmentListGlobal;
class OmSegmentListForGUI;
class OmSegmentListByMRU;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegment;

class OmSegmentLists {
 private:
  zi::rwmutex lock_;

  std::unique_ptr<OmSegmentListLowLevel> segmentListsLL_;

  std::shared_ptr<OmSegmentListGlobal> globalList_;
  std::shared_ptr<OmSegmentListForGUI> working_;
  std::shared_ptr<OmSegmentListForGUI> uncertain_;
  std::shared_ptr<OmSegmentListForGUI> valid_;

  std::unique_ptr<OmSegmentListByMRU> recent_;

  inline OmSegmentListForGUI* get(const om::common::SegListType type) {
    switch (type) {
      case om::common::SegListType::VALID:
        return valid_.get();
      case om::common::SegListType::WORKING:
        return working_.get();
      case om::common::SegListType::UNCERTAIN:
        return uncertain_.get();
      default:
        throw om::ArgException("unknown type");
    }
  }

 public:
  OmSegmentLists();
  ~OmSegmentLists();

  OmSegmentListLowLevel* LowLevelList() { return segmentListsLL_.get(); }

  void RefreshGUIlists();

  void TouchRecent(OmSegment* seg);

  void Swap(std::shared_ptr<OmSegmentListForGUI>& list);
  void Swap(std::shared_ptr<OmSegmentListGlobal>& globalList);

  size_t Size(const om::common::SegListType type);
  size_t SizeRecent();

  uint64_t GetNumTopLevelSegs();
  int64_t TotalNumVoxels();
  int64_t NumVoxels(const om::common::SegListType type);

  std::shared_ptr<GUIPageOfSegments> GetSegmentGUIPageRecent(
      const GUIPageRequest& request);

  std::shared_ptr<GUIPageOfSegments> GetSegmentGUIPage(
      const om::common::SegListType type, const GUIPageRequest& request);

  om::common::SegID GetNextSegIDinWorkingList(const SegmentDataWrapper&);
  om::common::SegID GetNextSegIDinWorkingList(const SegmentationDataWrapper&);

  int64_t GetSizeWithChildren(const om::common::SegID segID);
  int64_t GetSizeWithChildren(OmSegment* seg);

  int64_t GetNumChildren(const om::common::SegID segID);
  int64_t GetNumChildren(OmSegment* seg);
};
