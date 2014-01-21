#include "handler/handler.h"
#include "volume/segmentation.h"
#include "chunk/voxelGetter.hpp"
#include "utility/volumeWalker.hpp"

#include <zi/disjoint_sets/disjoint_sets.hpp>

namespace om {
namespace handler {

struct CompareBBox {
  bool operator()(const coords::DataBbox& a, const coords::DataBbox& b) {
    auto adim = a.getDimensions();
    auto bdim = b.getDimensions();
    return adim.x * adim.y * adim.z < bdim.x * bdim.y * bdim.z;
  }
};

typedef std::map<coords::DataBbox, std::pair<common::SegID, common::SegID>,
                 CompareBBox> pairwise_overlap_map;

pairwise_overlap_map FindPairwiseOverlaps(
    const common::SegIDSet& set, const volume::Segmentation& segmentation) {
  pairwise_overlap_map pairwiseOverlaps;
  for (const auto& a : set) {
    auto segA = segmentation.SegData()[a];
    for (const auto& b : set) {
      if (a == b) {
        continue;
      }
      auto segB = segmentation.SegData()[b];
      coords::DataBbox overlap(segA.bounds, segmentation.Coords(), 0);
      overlap.intersect(
          coords::DataBbox(segB.bounds, segmentation.Coords(), 0));
      // Offset by one to cover off-by-one errors which were causing segs to not
      // be grouped.
      overlap.setMin(overlap.getMin() - Vector3i::ONE);
      overlap.setMax(overlap.getMax() + Vector3i::ONE);
      pairwiseOverlaps[overlap] = std::make_pair(a, b);
    }
  }
  return pairwiseOverlaps;
}

std::vector<std::set<int>> ConnectedComponents(
    const common::SegIDSet& idSet, const pairwise_overlap_map& map,
    chunk::Voxels<uint32_t>& voxels, const volume::Segmentation& segmentation) {
  zi::disjoint_sets<uint32_t> sets(segmentation.SegData().size());

  for (const auto& overlap : map) {
    const auto& bounds = overlap.first;
    const auto& pair = overlap.second;

    if (sets.find_set(pair.first) == sets.find_set(pair.second)) {
      continue;
    }

    utility::VolumeWalker<uint32_t> walker(bounds, voxels);
    common::SegIDSet first;
    first.insert(pair.first);
    const common::SegID& second = pair.second;
    bool join =
        !walker.true_foreach_voxel_in_set(
             first, [&voxels, second](const coords::Data& dc, uint32_t) {
               return !(voxels.GetValue(dc + Vector3i(1, 0, 0)) == second ||
                        voxels.GetValue(dc + Vector3i(0, 1, 0)) == second ||
                        voxels.GetValue(dc + Vector3i(0, 0, 1)) == second ||
                        voxels.GetValue(dc + Vector3i(-1, 0, 0)) == second ||
                        voxels.GetValue(dc + Vector3i(0, -1, 0)) == second ||
                        voxels.GetValue(dc + Vector3i(0, 0, -1)) == second);
             });

    if (join) {
      sets.join(sets.find_set(pair.first), sets.find_set(pair.second));
    }
  }
  std::vector<std::set<int>> ret;
  std::map<int, int> mapping;
  for (auto segID : idSet) {
    auto component = sets.find_set(segID);
    if (mapping.find(component) == mapping.end()) {
      mapping[component] = ret.size();
      ret.emplace_back();
    }
    ret[mapping[component]].insert(segID);
  }

  return ret;
}

void get_connected_groups(
    std::vector<server::group>& _return, const volume::Segmentation& vol,
    const std::unordered_map<int, common::SegIDSet>& groups) {
  log_debugs << "Starting Comparison Task.";

  // all is the group of all segments selected by anyone.  It's size is
  // totalSize.
  server::group all;
  all.type = server::groupType::ALL;
  size_t totalSize = 0;
  all.groups.emplace_back();

  // agreed is the group of all the segments selected by everyone.  It's size is
  // agreedSize.
  server::group agreed;
  agreed.type = server::groupType::AGREED;
  size_t aggreedSize = 0;
  agreed.groups.emplace_back();

  // partial is the group of all segments which are agreed on by some but not
  // all of the users.
  server::group partial;
  partial.type = server::groupType::PARTIAL;
  partial.groups.emplace_back();

  // dust is the group of all segments which are grouped into small groups.
  // Small is defined as a percentage (DUST_THRESHOLD) of the totalSize.
  const double DUST_THRESHOLD = 0.01;
  server::group dust;
  dust.type = server::groupType::DUST;
  dust.groups.emplace_back();

  // Here's the plan:
  //
  // 1. Label each segid with a bit flag saying which users selected it
  // (segToFlag).  Keep track of which user is represented by each flag
  // (flagToUserid).
  std::unordered_map<common::SegID, int> segToFlag;
  std::unordered_map<int, int> flagToUserid;
  // agreedFlag is the flag with all the bits set.
  int agreedFlag = 0;

  // 2. Take segToFlag, and group those segments into one set for each
  // flag.
  std::unordered_map<int, common::SegIDSet> flagToSet;

  // 3. Take flagToSet and we make one set for each connected component for a
  // given flag.  Go through all the resultant components.  The small ones are
  // clumped into dust.  The rest get their own group in _return.

  log_debugs << "1. Flag Segments.";
  int uid = 1;
  for (const auto& group : groups) {
    agreedFlag |= uid;
    flagToUserid[uid] = group.first;
    for (const auto& segID : group.second) {
      if (segToFlag.find(segID) == segToFlag.end()) {
        segToFlag[segID] = 0;
      }
      segToFlag[segID] |= uid;
    }
    uid <<= 1;
  }

  log_debugs << "2. Group by Flag & Compute Total Size.";
  for (const auto& iter : segToFlag) {
    if (iter.first > vol.SegData().size()) {
      // Discard invalid segIDs
      continue;
    }
    all.groups.front().insert(iter.first);
    flagToSet[iter.second].insert(iter.first);
    totalSize += vol.SegData()[iter.first].size;
  }

  log_debugs << "3. Connected Components by flag.";

  chunk::Voxels<uint32_t> voxels(vol.ChunkDS(), vol.Coords());
  for (const auto& iter : flagToSet) {
    const int& flag = iter.first;
    const common::SegIDSet& set = iter.second;

    // agreed set is special. Calculate percent of total size and join all
    // together normally.
    if (flag == agreedFlag) {
      for (const auto& id : set) {
        aggreedSize += vol.SegData()[id].size;
      }
      agreed.groups.front().insert(set.begin(), set.end());
      continue;
    }

    auto flagUIDpair = flagToUserid.find(flag);
    // Check for partial agreement
    if (flagUIDpair == flagToUserid.end()) {
      partial.groups.front().insert(set.begin(), set.end());
      continue;
    }

    auto uid = flagUIDpair->second;

    const auto pairwiseOverlaps = FindPairwiseOverlaps(set, vol);
    server::group g;
    g.user_id = uid;
    g.type = server::groupType::USER;
    auto components = ConnectedComponents(set, pairwiseOverlaps, voxels, vol);

    for (const auto& c : components) {
      size_t size = 0;
      for (auto segID : c) {
        size += vol.SegData()[segID].size;
      }
      if ((double)size / totalSize < DUST_THRESHOLD) {
        dust.groups.front().insert(c.begin(), c.end());
      } else {
        g.groups.push_back(c);
      }
    }
    _return.push_back(g);
  }
  _return.push_back(all);
  _return.push_back(agreed);
  _return.push_back(partial);
  _return.push_back(dust);
}
}
}
