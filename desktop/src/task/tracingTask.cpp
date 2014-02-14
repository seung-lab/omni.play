#include "tracingTask.h"
#include "project/omProject.h"
#include "system/omAppState.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "segment/omSegments.h"
#include "segment/selection.hpp"
#include "segment/omSegmentUtils.hpp"
#include "system/account.h"
#include "network/http/http.hpp"
#include "gui/mainWindow/mainWindow.h"
#include "viewGroup/omViewGroupState.h"
#include "gui/viewGroup/viewGroup.h"
#include "segment/omSegmentCenter.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegmentSelector.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "segment/lowLevel/omSegmentChildren.hpp"

namespace om {
namespace task {

TracingTask::TracingTask()
    : data_{0, 0, "", common::SegIDSet{}, std::vector<SegGroup>{}} {}

TracingTask::TracingTask(uint32_t id, uint32_t cellId, const std::string& path,
                         common::SegIDSet&& seed)
    : data_{id,
            cellId,
            path,
            seed,
            std::vector<SegGroup>{{"Seed", SegGroup::GroupType::SEED, seed}}} {}

TracingTask::~TracingTask() {}

bool TracingTask::Start() {
  if (data_.path.empty() ||
      !OmAppState::OpenProject(data_.path, om::system::Account::username())) {
    return false;
  }
  const SegmentationDataWrapper sdw(1);
  if (!sdw.IsValidWrapper()) {
    return false;
  }
  sdw.Segments()->UpdateSegmentSelection(data_.seed, true);
  return true;
}

bool TracingTask::Submit() {
  if (!OmProject::IsOpen()) {
    return false;
  }

  std::unordered_map<common::SegID, int> segIDs;
  const SegmentationDataWrapper sdw(1);
  if (!sdw.IsValidWrapper()) {
    return false;
  }
  const auto& rootIDs = sdw.Segments()->Selection().GetSelectedSegmentIDs();
  const auto& segments =
      OmSegmentUtils::GetAllChildrenSegments(sdw.Segments(), rootIDs);
  for (OmSegment* seg : *segments) {
    segIDs[seg->value()] = 1;
  }

  auto uri = system::Account::endpoint() + "/1.0/cube/submit";
  network::HTTP::POST(uri, std::make_pair("id", data_.id),
                      std::make_pair("plane", "xy"),
                      std::make_pair("segments", segIDs),
                      std::make_pair("reap", 0), std::make_pair("status", 0));
  return true;
}

}  // namespace om::task::
}  // namespace om::
