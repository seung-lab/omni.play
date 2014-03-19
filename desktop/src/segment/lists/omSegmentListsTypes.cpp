#include "segment/lists/omSegmentListsTypes.hpp"

std::shared_ptr<GUIPageOfSegments> om::segLists::getPage(
    const std::vector<SegInfo> list, const uint32_t startIndex,
    const uint32_t numToGet) {
  auto ret = std::make_shared<GUIPageOfSegments>();

  if (list.empty()) {
    return ret;
  }

  if (startIndex > list.size()) {
    log_errors << startIndex << " > " << list.size();
    return ret;
  }

  ret->pageNum = startIndex / numToGet;

  uint32_t endIndex = startIndex + numToGet;
  if (endIndex > list.size()) {
    endIndex = list.size();
  }

  ret->segs.reserve(endIndex - startIndex);

  for (auto i = startIndex; i < endIndex; ++i) {
    ret->segs.push_back(list[i]);
  }

  return ret;
}
