#include "handler/handler.h"
#include "volume/segmentation.h"
#include "chunk/voxelGetter.hpp"
#include "utility/volumeWalker.hpp"

#include <zi/disjoint_sets/disjoint_sets.hpp>

namespace om {
namespace handler {

typedef std::multimap<size_t, std::tuple<coords::DataBbox, common::SegID,
                                         common::SegID>> pairwise_overlap_map;

pairwise_overlap_map FindPairwiseOverlaps(
    const std::set<int>& set, const volume::Segmentation& segmentation) {
  pairwise_overlap_map pairwiseOverlaps;
  for (const auto& a : set) {
    for (const auto& b : set) {
      if (a <= b) {
        continue;
      }
      auto segA = segmentation.SegData()[a];
      auto segB = segmentation.SegData()[b];
      // Start with B and increase the size by 1 to account for touching but not
      // overlapping bboxes.  Must do B first because we iterate through the
      // volume looking for A.
      coords::DataBbox overlap(segB.bounds, segmentation.Coords(), 0);
      overlap.setMin(overlap.getMin() - Vector3i::ONE);
      overlap.setMax(overlap.getMax() + Vector3i::ONE);

      overlap.intersect(
          coords::DataBbox(segA.bounds, segmentation.Coords(), 0));

      if (overlap.isEmpty()) {
        continue;
      }
      auto dims = overlap.getDimensions();
      pairwiseOverlaps.emplace(dims.x * dims.y * dims.z,
                               std::make_tuple(overlap, a, b));
    }
  }
  return pairwiseOverlaps;
}

std::vector<std::set<int>> ConnectedComponents(
    const std::set<int>& idSet, const pairwise_overlap_map& map,
    const volume::Segmentation& segmentation) {
  zi::disjoint_sets<uint32_t> sets(segmentation.SegData().size());

  for (const auto& overlap : map) {
    const auto& bounds = std::get<0>(overlap.second);
    const auto& segA = std::get<1>(overlap.second);
    const auto& segB = std::get<2>(overlap.second);

    if (sets.find_set(segA) == sets.find_set(segB)) {
      continue;
    }

    bool join = false;
    for (auto iter : segmentation.SegIterate(segA, bounds)) {
      auto n = iter.neighbor(Vector3i(1, 0, 0));
      if ((join = (bool)n && n->value() == segB)) break;

      n = iter.neighbor(Vector3i(0, 1, 0));
      if ((join = (bool)n && n->value() == segB)) break;

      n = iter.neighbor(Vector3i(0, 0, 1));
      if ((join = (bool)n && n->value() == segB)) break;

      n = iter.neighbor(Vector3i(-1, 0, 0));
      if ((join = (bool)n && n->value() == segB)) break;

      n = iter.neighbor(Vector3i(0, -1, 0));
      if ((join = (bool)n && n->value() == segB)) break;

      n = iter.neighbor(Vector3i(0, 0, -1));
      if ((join = (bool)n && n->value() == segB)) break;
    }

    if (join) {
      sets.join(sets.find_set(segA), sets.find_set(segB));
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

void Subset(std::vector<server::group>& _return, int uid,
            server::groupType::type groupType, const std::set<int>& set,
            const volume::Segmentation& vol, size_t totalSize) {
  // Small is defined as a percentage (DUST_THRESHOLD) of the size of all
  // segments.
  const double DUST_THRESHOLD = 0.01;

  server::group dust;
  dust.user_id = uid;
  dust.type = groupType;
  dust.dust = true;
  dust.size = 0;

  const auto pairwiseOverlaps = FindPairwiseOverlaps(set, vol);
  auto components = ConnectedComponents(set, pairwiseOverlaps, vol);

  for (const auto& c : components) {
    server::group g;
    g.user_id = uid;
    g.type = groupType;
    g.dust = false;
    g.size = 0;

    for (auto segID : c) {
      g.size += vol.SegData()[segID].size;
    }
    if ((double)g.size / totalSize < DUST_THRESHOLD) {
      dust.segments.insert(c.begin(), c.end());
      dust.size += g.size;
    } else {
      g.segments = c;
      _return.push_back(g);
    }
  }
  _return.push_back(dust);
}

void get_connected_groups(
    std::vector<server::group>& _return, const volume::Segmentation& vol,
    const std::unordered_map<int, common::SegIDSet>& groups) {
  log_infos << "Making Comparison Task.";

  // Here's the plan:
  //
  // 1. Label each segid with a bit flag saying which users selected it
  // (segToFlag).  Keep track of which user is represented by each flag
  // (flagToUserid).
  std::unordered_map<common::SegID, int> segToFlag;
  std::unordered_map<int, int> flagToUserid;
  std::unordered_map<int, int> flagToMissingUserid;
  // agreedFlag is the flag with all the bits set.
  int agreedFlag = 0;

  // 2. Take segToFlag, and group those segments into one set for each
  // flag.
  std::unordered_map<int, std::set<int>> flagToSet;

  // 3. Take flagToSet and we make one set for each connected component for a
  // given flag.  Go through all the resultant components.  The small ones are
  // clumped into dust ona per user basis.  The rest get their own group in
  // _return.

  log_debugs << "1. Flag Segments.";
  int flag = 1;
  for (const auto& group : groups) {
    auto uid = group.first;
    if (uid < 2) {
      // Skip (existing comparison) validation by the default user.
      log_errors << "get_connected_groups: received user_id=" << uid
                 << " validation. Manually modified database record?";
      continue;
    }
    agreedFlag |= flag;
    flagToUserid[flag] = uid;
    for (const auto& segID : group.second) {
      if (segToFlag.find(segID) == segToFlag.end()) {
        segToFlag[segID] = 0;
      }
      segToFlag[segID] |= flag;
    }
    flag <<= 1;
  }

  for (const auto& iter : flagToUserid) {
    flagToMissingUserid[agreedFlag ^ iter.first] = iter.second;
  }

  server::group all;
  all.user_id = 0;
  all.type = server::groupType::ALL;
  all.dust = false;
  all.size = 0;

  log_debugs << "2. Group by Flag & Compute Total Size.";
  for (const auto& iter : segToFlag) {
    if (iter.first > vol.SegData().size()) {
      // Discard invalid segIDs
      continue;
    }
    all.segments.insert(iter.first);
    flagToSet[iter.second].insert(iter.first);
    all.size += vol.SegData()[iter.first].size;
  }
  _return.push_back(all);

  log_debugs << "3. Connected Components by flag.";

  std::set<int> partialIDs;

  for (const auto& iter : flagToSet) {
    const int& flag = iter.first;
    const std::set<int>& set = iter.second;

    // agreed set is special. Calculate percent of total size and join all
    // together normally.
    if (flag == agreedFlag) {
      server::group agreed;
      agreed.type = server::groupType::AGREED;
      agreed.size = 0;
      for (const auto& id : set) {
        agreed.size += vol.SegData()[id].size;
      }
      agreed.segments = set;
      _return.push_back(agreed);
      continue;
    }

    auto flagUIDmissed = flagToMissingUserid.find(flag);
    if (flagUIDmissed != flagToMissingUserid.end()) {
      Subset(_return, flagUIDmissed->second, server::groupType::USER_MISSED,
             set, vol, all.size);
      continue;
    }

    auto flagUIDfound = flagToUserid.find(flag);
    if (flagUIDfound != flagToUserid.end()) {
      Subset(_return, flagUIDfound->second, server::groupType::USER_FOUND, set,
             vol, all.size);
      continue;
    }

    partialIDs.insert(set.begin(), set.end());
  }
  Subset(_return, 0, server::groupType::PARTIAL, partialIDs, vol, all.size);
}
}
}
