#include "task/reapingTask.h"
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
#include "users/omUsers.h"

namespace om {
namespace task {

ReapingTask::ReapingTask() {}
ReapingTask::~ReapingTask() {}

bool ReapingTask::Start() {
  if (data_.path.empty() ||
      !OmAppState::OpenProject(data_.path, om::users::defaultUser)) {
    return false;
  }
  const SegmentationDataWrapper sdw(1);
  if (!sdw.IsValidWrapper()) {
    return false;
  }

  // reset user edges etc
  sdw.GetSegmentationPtr()->ClearUserChangesAndSave();

  common::SegIDSet truth;
  for (auto& seg : data_.aggregate.Segments) {
    truth.insert(seg.first);
  }
  sdw.Segments()->JoinTheseSegments(truth);

  sdw.Segments()->UpdateSegmentSelection(truth, true);
  return true;
}

bool ReapingTask::Submit() {
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
                      std::make_pair("reap", 1), std::make_pair("status", 0));
  return true;
}

}  // namespace om::task::
}  // namespace om::
