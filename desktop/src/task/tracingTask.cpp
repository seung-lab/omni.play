#include "tracingTask.h"
#include "project/omProject.h"
#include "system/omAppState.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "segment/omSegments.h"
#include "segment/selection.hpp"
#include "segment/lowLevel/children.hpp"
#include "system/account.h"
#include "network/http/http.hpp"

namespace om {
namespace task {

TracingTask::TracingTask(uint32_t id, uint32_t cellId, const std::string& path,
                         common::SegIDSet&& seed)
    : id_(id), cellId_(cellId), path_(path), seed_(seed) {}

TracingTask::~TracingTask() {}

bool TracingTask::Start() {
  if (!OmAppState::OpenProject(path_)) {
    return false;
  }
  const SegmentationDataWrapper sdw(1);
  sdw.Segments()->Selection().UpdateSegmentSelection(seed_, true);
  return true;
}

bool TracingTask::Submit() {
  if (!OmProject::IsOpen()) {
    return false;
  }

  const SegmentationDataWrapper sdw(1);

  const auto& rootIDs = sdw.Segments()->Selection().GetSelectedSegmentIDs();
  auto& childrenTable = sdw.Segments()->Children();

  std::unordered_map<common::SegID, int> segIDs;
  for (auto id : rootIDs) {
    segIDs[id] = 1;
    const auto& segments = childrenTable.GetChildren(id);
    for (OmSegment* seg : segments) {
      segIDs[seg->value()] = 1;
    }
  }

  auto uri = system::Account::endpoint() + "/api/v1/task";
  network::HTTP::POST(
      uri, std::make_pair("id", id_), std::make_pair("plane", "xy"),
      std::make_pair("segments", segIDs), std::make_pair("auth", 0),
      std::make_pair("status", 0));
  return true;
}

}  // namespace om::task::
}  // namespace om::
