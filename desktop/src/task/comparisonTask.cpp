#include "comparisonTask.h"
#include "chunk/voxelGetter.hpp"
#include "project/omProject.h"
#include "system/omAppState.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "volume/omSegmentation.h"
#include "segment/types.hpp"
#include "segment/omSegments.h"
#include "segment/selection.hpp"
#include "system/account.h"
#include "network/http/http.hpp"
#include "system/cache/omCacheManager.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegmentSelector.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "segment/lowLevel/omSegmentChildren.hpp"

namespace om {
namespace task {

ComparisonTask::ComparisonTask(uint32_t id, uint32_t cellId,
                               const std::string& path,
                               UserSegContainer&& userSegs)
    : id_(id), cellId_(cellId), path_(path), userSegs_(userSegs) {}

ComparisonTask::~ComparisonTask() {}

bool ComparisonTask::Start() {
  if (!OmAppState::OpenProject(path_)) {
    return false;
  }
  const SegmentationDataWrapper sdw(1);
  if (!sdw.IsValidWrapper()) {
    return false;
  }

  // Identify for each segment the group of users that picked it
  // (generate a bitmap for all picked segIDs, with each user taking one bit).
  // Also construct a set of all segIDs.
  std::unordered_map<common::SegID, int> segFlags;
  common::SegIDSet allSeeds;
  int allUsers = 0;
  int uid = 1;
  for (const auto& segs : userSegs_) {
    allUsers |= uid;
    auto it = allSeeds.begin();
    for (const auto& segID : segs) {
      segFlags[segID] |= uid;
      it = allSeeds.insert(it, segID);
    }
    uid <<= 1;
  }

  OmSegmentation* segmentation = sdw.GetSegmentationPtr();
  const auto& volumeSystem = segmentation->Coords();
  int chunkDim = volumeSystem.GetChunkDimension();

  // Brute force though the entire volume to find connected segments.
  // Adjacent segments will be joined together only if they are selected
  // by the exact same group of users.
  using om::common::SegID;
  std::set<std::pair<SegID, SegID>> edgesToAdd;
  auto testAndAddEdge = [&](SegID v1, SegID v2) {
    if (v1 && v2 && v1 != v2 && segFlags.find(v1) != segFlags.end() &&
        segFlags.find(v2) != segFlags.end() &&
        segFlags.find(v1)->second == segFlags.find(v2)->second) {
      edgesToAdd.insert({v1, v2});
    }
  };
  const auto& chunks = segmentation->GetMipChunkCoords(0);
  for (auto& chunk : *chunks) {
    if (!chunkHasUserSegments(segmentation->UniqueValuesDS(), chunk,
                              segFlags)) {
      continue;
    }

    const auto& data = chunk.toDataCoord(volumeSystem);
    for (int x = 0; x < chunkDim; ++x) {
      for (int y = 0; y < chunkDim; ++y) {
        for (int z = 0; z < chunkDim; ++z) {
          const om::dataCoord& coord = data + Vector3i(x, y, z);
          auto v1 = segmentation->GetVoxelValue(coord.toGlobalCoord());
          auto v2 = segmentation->GetVoxelValue(
              (coord + Vector3i(1, 0, 0)).toGlobalCoord());
          testAndAddEdge(v1, v2);
          v2 = segmentation->GetVoxelValue(
              (coord + Vector3i(0, 1, 0)).toGlobalCoord());
          testAndAddEdge(v1, v2);
          v2 = segmentation->GetVoxelValue(
              (coord + Vector3i(0, 0, 1)).toGlobalCoord());
          testAndAddEdge(v1, v2);
        }
      }
    }
  }
  for (auto& edge : edgesToAdd) {
    sdw.Segments()->JoinEdge(OmSegmentEdge(edge.first, edge.second, 1.0));
  }
  common::SegIDSet consensusRoots;
  for (const auto& flag : segFlags) {
    if (flag.second == allUsers) {
      consensusRoots.insert(sdw.Segments()->findRootID(flag.first));
    }
  }
  for (const auto& root : consensusRoots) {
    sdw.Segments()->GetSegment(root)->SetColor(om::common::Color{255, 100, 0});
  }

  OmCacheManager::TouchFreshness();
  sdw.SegmentLists()->RefreshGUIlists();
  sdw.Segments()->UpdateSegmentSelection(allSeeds, true);

  return true;
}

bool ComparisonTask::chunkHasUserSegments(
    OmChunkUniqueValuesManager& uniqueValues, const om::chunkCoord& chunk,
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

  const SegmentationDataWrapper sdw(1);

  const auto& rootIDs = sdw.Segments()->Selection().GetSelectedSegmentIDs();
  auto& childrenTable = *sdw.Segments()->Children();

  std::unordered_map<common::SegID, int> segIDs;
  for (auto id : rootIDs) {
    segIDs[id] = 1;
    const auto& segments = childrenTable.GetChildren(id);
    for (OmSegment* seg : segments) {
      segIDs[seg->value()] = 1;
    }
  }

  auto uri = system::Account::endpoint() + "/api/v1/task";
  network::HTTP::POST(uri, std::make_pair("id", id_),
                      std::make_pair("plane", "xy"),
                      std::make_pair("segments", segIDs),
                      std::make_pair("auth", 0), std::make_pair("status", 0));
  return true;
}

}  // namespace om::task::
}  // namespace om::
