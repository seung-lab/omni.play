#include "chunk/voxelGetter.hpp"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "comparisonTask.h"
#include "network/http/http.hpp"
#include "project/omProject.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/lowLevel/omSegmentChildren.hpp"
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
                               UserSegContainer&& userSegs)
    : id_(id), cellId_(cellId), path_(path), userSegs_(userSegs) {}

ComparisonTask::~ComparisonTask() {}

struct CompareBBox {
  bool operator()(const coords::DataBbox& a, const coords::DataBbox& b) {
    auto adim = a.getDimensions();
    auto bdim = b.getDimensions();
    return adim.x * adim.y * adim.z < bdim.x * bdim.y * bdim.z;
  }
};

typedef std::map<coords::DataBbox, std::pair<common::SegID, common::SegID>,
                 CompareBBox> pairwise_overlap_map;

pairwise_overlap_map FindPairwiseOverlaps(const common::SegIDSet& set,
                                          OmSegments* segments) {
  pairwise_overlap_map pairwiseOverlaps;
  for (const auto& a : set) {
    auto segA = segments->GetSegment(a);
    if (!segA) {
      continue;
    }
    for (const auto& b : set) {
      if (a == b) {
        continue;
      }
      auto segB = segments->GetSegment(b);
      if (!segB) {
        continue;
      }
      coords::DataBbox overlap = segA->BoundingBox();
      overlap.intersect(segB->BoundingBox());
      overlap.setMin(overlap.getMin() - Vector3i::ONE);
      overlap.setMax(overlap.getMax() + Vector3i::ONE);
      pairwiseOverlaps[overlap] = std::make_pair(a, b);
    }
  }
  return pairwiseOverlaps;
}

void JoinAdjacent(const pairwise_overlap_map& map,
                  chunk::Voxels<uint32_t>& voxels, OmSegments* segments) {
  for (const auto& overlap : map) {
    const auto& bounds = overlap.first;
    const auto& pair = overlap.second;

    if (segments->findRootID(pair.first) == segments->findRootID(pair.second)) {
      continue;
    }

    utility::VolumeWalker<uint32_t> walker(bounds, voxels);
    common::SegIDSet first;
    first.insert(pair.first);
    const common::SegID& second = pair.second;
    bool join =
        !walker.true_foreach_voxel_in_set(
             first, [&voxels, &second](const coords::Data& dc, uint32_t) {
               return !(voxels.GetValue(dc + Vector3i(1, 0, 0)) == second ||
                        voxels.GetValue(dc + Vector3i(0, 1, 0)) == second ||
                        voxels.GetValue(dc + Vector3i(0, 0, 1)) == second ||
                        voxels.GetValue(dc + Vector3i(-1, 0, 0)) == second ||
                        voxels.GetValue(dc + Vector3i(0, -1, 0)) == second ||
                        voxels.GetValue(dc + Vector3i(0, 0, -1)) == second);
             });

    if (join) {
      segments->JoinEdge(OmSegmentEdge(pair.first, pair.second, 2.0));
    }
  }
}

std::string GetName(
    int flag, const std::unordered_map<int, std::string>& flagToUsername) {
  std::string name = "";
  for (auto& pair : flagToUsername) {
    if ((pair.first & flag) == pair.first) {
      if (!name.empty()) {
        name += " + " + pair.second;
      } else {
        name += pair.second;
      }
    }
  }
  return name;
}

bool ComparisonTask::Start() {
  if (!OmAppState::OpenProject(path_, om::users::defaultUser)) {
    return false;
  }
  const SegmentationDataWrapper sdw(1);
  if (!sdw.IsValidWrapper()) {
    return false;
  }

  log_debugs << "Starting Comparison Task.";

  OmSegmentation* segmentation = sdw.GetSegmentationPtr();
  OmSegments* segments = segmentation->Segments();

  // Identify for each segment the group of users that picked it
  // (generate a bitmap for all picked segIDs, with each user taking one bit).
  // Also construct a set of all segIDs.
  log_debugs << "Find Flags by SegID.";

  std::unordered_map<common::SegID, int> segToFlag;
  std::unordered_map<int, std::string> flagToUsername;
  common::SegIDSet all;
  int allUsers = 0;
  int uid = 1;
  for (const auto& segs : userSegs_) {
    allUsers |= uid;
    flagToUsername[uid] = segs.first;
    for (const auto& segID : segs.second) {
      all.insert(segID);
      if (segToFlag.find(segID) == segToFlag.end()) {
        segToFlag[segID] = 0;
      }
      segToFlag[segID] |= uid;
    }
    uid <<= 1;
    if (uid == 4) {
      break;
    }
  }

  // reset user edges etc
  log_debugs << "Clear Selection";
  segmentation->ClearUserChangesAndSave();
  userSegs_.clear();

  // Group into sets by flag
  log_debugs << "Group by Flag.";
  std::unordered_map<int, common::SegIDSet> flagToSet;
  size_t totalSize = 0;
  for (const auto& iter : segToFlag) {
    auto seg = segments->GetSegment(iter.first);
    if (!seg) {
      continue;
    }
    totalSize += seg->size();
    flagToSet[iter.second].insert(iter.first);
  }

  // Join adjacent segments within a group.
  log_debugs << "Join segs within groups.";
  chunk::Voxels<uint32_t> voxels(segmentation->CommonSegmentation().ChunkDS(),
                                 segmentation->Coords());

  std::map<std::string, size_t> groupSizes;

  for (const auto& iter : flagToSet) {
    const int& flag = iter.first;
    const common::SegIDSet& set = iter.second;

    // agreed set is special. Calculate percent of total size and join all
    // together normally.
    if (flag == allUsers) {
      size_t aggreedSize = 0;
      for (const auto& id : set) {
        auto seg = segments->GetSegment(id);
        if (!seg) {
          continue;
        }
        seg->SetColor(om::common::Color{255, 100, 0});
        aggreedSize += seg->size();
      }
      std::string name("agreed: ");
      name += std::to_string((float)aggreedSize / (float)totalSize);
      log_debugs << "Added: " << name << "; size = " << set.size();
      userSegs_[name] = set;
      segments->JoinTheseSegments(set);
      continue;
    }

    // Find the overlap of the bounding boxes of all pairs of segments.
    const auto pairwiseOverlaps = FindPairwiseOverlaps(set, segments);

    // Explore each overlap region from smallest to largest for touching
    // segments.
    JoinAdjacent(pairwiseOverlaps, voxels, segments);

    // create new segment groups named by the users who picked them and the
    // root.
    const auto name = GetName(flag, flagToUsername);
    for (const auto& id : set) {
      auto seg = segments->GetSegment(iter.first);
      if (!seg) {
        continue;
      }
      auto rootID = segments->findRootID(id);
      const auto groupName = name + " : " + std::to_string(rootID);
      userSegs_[groupName].insert(id);
      groupSizes[groupName] += seg->size();
    }
  }

  // Clump dust groups.
  const double DUST_THRESHOLD = 0.01;
  const auto groups = userSegs_;
  for (auto& group : groups) {
    if (group.first.find("agreed") != std::string::npos) {
      continue;
    }
    if ((double)groupSizes[group.first] / totalSize < DUST_THRESHOLD) {
      userSegs_["dust"].insert(group.second.begin(), group.second.end());
      userSegs_.erase(group.first);
    }
  }

  OmCacheManager::TouchFreshness();
  sdw.SegmentLists()->RefreshGUIlists();
  userSegs_["all"] = all;
  common::SegIDSet allRoots;
  for (const auto& id : all) {
    auto rootID = segments->findRootID(id);
    if (rootID) {
      allRoots.insert(rootID);
    }
  }

  segments->UpdateSegmentSelection(allRoots, true);

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
