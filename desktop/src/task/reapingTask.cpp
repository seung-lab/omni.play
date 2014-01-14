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

ReapingTask::ReapingTask() : id_(0), cellId_(0) {}

ReapingTask::ReapingTask(uint32_t id, uint32_t cellId, const std::string& path,
                         common::SegIDSet&& seed, Aggregate&& aggregate)
    : id_(id),
      cellId_(cellId),
      path_(path),
      seed_(seed),
      aggregate_(aggregate) {}

ReapingTask::~ReapingTask() {}

bool ReapingTask::Start() {
  if (path_.empty() ||
      !OmAppState::OpenProject(path_, om::users::defaultUser)) {
    return false;
  }
  const SegmentationDataWrapper sdw(1);
  if (!sdw.IsValidWrapper()) {
    return false;
  }

  // reset user edges etc
  sdw.GetSegmentationPtr()->ClearUserChangesAndSave();

  common::SegIDSet truth;
  for (auto& seg : aggregate_.Segments) {
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

  auto uri = system::Account::endpoint() + "/api/v1/task";
  network::HTTP::POST(uri, std::make_pair("id", id_),
                      std::make_pair("plane", "xy"),
                      std::make_pair("segments", segIDs),
                      std::make_pair("auth", 1), std::make_pair("status", 0));
  return true;
}

}  // namespace om::task::
}  // namespace om::
