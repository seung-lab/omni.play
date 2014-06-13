#include "chunk/voxelGetter.hpp"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "comparisonTask.h"
#include "network/http/http.hpp"
#include "project/omProject.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/lowLevel/children.hpp"
#include "segment/omSegments.h"
#include "segment/omSegmentSelector.h"
#include "segment/omSegmentUtils.hpp"
#include "segment/selection.hpp"
#include "segment/types.hpp"
#include "system/account.h"
#include "system/cache/omCacheManager.h"
#include "system/omAppState.hpp"
#include "users/omUsers.h"
#include "utility/segmentationDataWrapper.hpp"
#include "utility/volumeWalker.hpp"
#include "volume/omSegmentation.h"
#include "volume/isegmentation.hpp"

namespace om {
namespace task {

ComparisonTask::ComparisonTask(uint32_t id, uint32_t cellId,
                               const std::string& path,
                               std::vector<SegGroup>&& namedGroups)
    : id_(id), cellId_(cellId), path_(path), namedGroups_(namedGroups) {}

ComparisonTask::~ComparisonTask() {}

bool ComparisonTask::Start() {
  if (!OmAppState::OpenProject(path_, om::users::defaultUser)) {
    return false;
  }
  const SegmentationDataWrapper sdw(1);
  if (!sdw.IsValidWrapper()) {
    return false;
  }

  log_debugs << "Starting Comparison Task.";

  OmSegmentation* segmentation = sdw.GetSegmentation();
  OmSegments& segments = segmentation->Segments();

  segmentation->ClearUserChangesAndSave();
  sdw.SegmentLists()->RefreshGUIlists();

  auto seedIter = std::find_if(
      namedGroups_.begin(), namedGroups_.end(),
      [](const SegGroup& g) { return g.type == SegGroup::GroupType::SEED; });
  auto agreedIter = std::find_if(
      namedGroups_.begin(), namedGroups_.end(),
      [](const SegGroup& g) { return g.type == SegGroup::GroupType::AGREED; });
  std::map<SegGroup::GroupType, std::string> typeString{
      {SegGroup::GroupType::SEED, "Seed"},
      {SegGroup::GroupType::AGREED, "Agreed"}};
  common::SegIDSet allRoots;
  for (auto iter : {seedIter, agreedIter}) {
    if (iter != namedGroups_.end()) {
      for (const auto& id : iter->segments) {
        if (id <= 0 || id > segments.maxValue()) {
          log_errors << "Invalid segment id " << id << " in "
                     << typeString[iter->type] << " segment group.";
          continue;
        }
        auto rootID = segments.FindRootID(id);
        if (rootID) {
          allRoots.insert(rootID);
        }
      }
    }
  }
  if (allRoots.size() > 0) {
    segments.Selection().UpdateSegmentSelection(allRoots, true);
  } else {
    // clear any pre-existing selection (which could exist if the previous task
    // happened to be in the same volume)
    segments.Selection().UpdateSegmentSelection({}, true);
    log_errors << "No segments in Seed segments group.";
  }

  om::event::Redraw2d();
  om::event::Redraw3d();
  return true;
}

bool ComparisonTask::chunkHasUserSegments(
    OmChunkUniqueValuesManager& uniqueValues, const om::coords::Chunk& chunk,
    const std::unordered_map<common::SegID, int>& segFlags) {

  const auto uv = uniqueValues.Get(chunk);
  for (auto& segID : uv) {
    if (segFlags.find(segID) != segFlags.end()) {
      return true;
    }
  }
  return false;
}

bool ComparisonTask::Submit() {
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
      OmSegmentUtils::GetAllChildrenSegments(*sdw.Segments(), rootIDs);
  for (OmSegment* seg : *segments) {
    segIDs[seg->value()] = 1;
  }

  auto uri = system::Account::endpoint("/1.0/task/submit");
  network::HTTP::POST(
      uri, std::make_pair("id", id_), std::make_pair("plane", "xy"),
      std::make_pair("segments", segIDs), std::make_pair("reap", "true"),
      std::make_pair("status", 0));
  return true;
}

}  // namespace om::task::
}  // namespace om::
