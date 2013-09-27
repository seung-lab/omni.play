#include "chunk/voxelGetter.hpp"
#include "common/common.h"
#include "handler/handler.h"
#include "volume/metadataManager.h"
#include "volume/segmentation.h"
#include "volume/volume.h"

#include <vector>
#include <set>
#include <string>
#include <unordered_map>
#include <sstream>

#include <zi/disjoint_sets/disjoint_sets.hpp>

namespace om {
namespace handler {

/*
 * Direction from Pre into Post
 */
enum class Direction {
  XMin,
  XMax,
  YMin,
  YMax,
  ZMin,
  ZMax,
};

void conditionalJoin(zi::disjoint_sets<uint32_t>& sets, uint32_t id1,
                     uint32_t id2) {
  uint32_t id1_rep = sets.find_set(id1);
  uint32_t id2_rep = sets.find_set(id2);
  sets.join(id1_rep, id2_rep);
}

inline Vector3i slab(const coords::GlobalBbox& bounds) {
  return Vector3i(bounds.getMax().x - bounds.getMin().x,
                  bounds.getMax().y - bounds.getMin().y,
                  bounds.getMax().z - bounds.getMin().z);
}

inline uint32_t toProxy(const coords::Global& g, const Vector3i& slab,
                        const coords::GlobalBbox& bounds) {
  auto min = bounds.getMin();
  return slab.x * slab.y * (g.z - min.z) + slab.x * (g.y - min.y) + g.x - min.x;
}

inline coords::Global fromProxy(const uint32_t proxy, const Vector3i& slab,
                                const coords::GlobalBbox& bounds) {
  auto min = bounds.getMin();
  return coords::Global(proxy % slab.x + min.x,
                        (proxy % (slab.x * slab.y)) / slab.x + min.y,
                        proxy / (slab.x * slab.y) + min.z);
}

inline Direction getDirection(const coords::GlobalBbox& pre,
                              const coords::GlobalBbox& post) {
  coords::GlobalBbox bounds = pre;
  bounds.intersect(post);
  assert(!bounds.isEmpty());

  if (bounds.getMin().x > post.getMin().x) {
    return Direction::XMin;
  }

  if (bounds.getMin().y > post.getMin().y) {
    return Direction::YMin;
  }

  if (bounds.getMin().z > post.getMin().z) {
    return Direction::ZMin;
  }

  if (bounds.getMax().x < post.getMax().x) {
    return Direction::XMax;
  }

  if (bounds.getMax().y < post.getMax().y) {
    return Direction::YMax;
  }

  return Direction::ZMax;
}

inline bool inCriticalRegion(const coords::Global& g,
                             const coords::GlobalBbox& bounds,
                             const Direction d) {
  const int FUDGE = 5;
  return ((d == Direction::XMin && g.x < bounds.getMin().x + FUDGE) ||
          (d == Direction::YMin && g.y < bounds.getMin().y + FUDGE) ||
          (d == Direction::ZMin && g.z < bounds.getMin().z + FUDGE) ||
          (d == Direction::XMax && g.x > bounds.getMax().x - FUDGE) ||
          (d == Direction::YMax && g.y > bounds.getMax().y - FUDGE) ||
          (d == Direction::ZMax && g.z > bounds.getMax().z - FUDGE));
}

std::map<int32_t, int32_t> makeSeed(
    const std::set<uint32_t>& bundle,
    std::unordered_map<uint32_t, int> mappingCounts,
    std::unordered_map<uint32_t, int> sizes) {
  const double FALSE_OBJ_RATIO_THR = 0.5;
  std::map<int32_t, int32_t> ret;
  uint32_t largest = 0;
  uint32_t largestSize = 0;

  for (auto& seg : bundle) {
    if (((double)mappingCounts[seg]) / ((double)sizes[seg]) >=
        FALSE_OBJ_RATIO_THR) {
      ret[seg] = sizes[seg];
    }
    if (sizes[seg] > largestSize) {
      largest = seg;
      largestSize = sizes[seg];
    }
  }

  if (ret.size() == 0) {
    ret[largest] = largestSize;
  }

  return ret;
}

coords::GlobalBbox getBounds(const coords::GlobalBbox& pre,
                             const coords::GlobalBbox& post,
                             const Direction d) {
  auto bounds = pre;
  bounds.intersect(post);

  // Trim on preside
  const int FUDGE = 5;
  auto min = bounds.getMin();
  auto max = bounds.getMax();

  switch (d) {
    case Direction::XMin:
      bounds.setMax(Vector3f(max.x - FUDGE, max.y, max.z));
      break;
    case Direction::YMin:
      bounds.setMax(Vector3f(max.x, max.y - FUDGE, max.z));
      break;
    case Direction::ZMin:
      bounds.setMax(Vector3f(max.x, max.y, max.z - FUDGE));
      break;
    case Direction::XMax:
      bounds.setMin(Vector3f(min.x + FUDGE, min.y, min.z));
      break;
    case Direction::YMax:
      bounds.setMin(Vector3f(min.x, min.y + FUDGE, min.z));
      break;
    case Direction::ZMax:
      bounds.setMin(Vector3f(min.x, min.y, min.z + FUDGE));
      break;
  }

  return bounds;
}

void get_seeds(std::vector<std::map<int32_t, int32_t>>& seeds,
               const volume::Segmentation& pre,
               const std::set<int32_t>& selected,
               const volume::Segmentation& post) {
  log_infos(unknown) << "Getting Seeds\n" << pre.Endpoint() << '\n'
                     << post.Endpoint();

  auto preBounds = pre.Metadata().bounds();
  auto postBounds = post.Metadata().bounds();
  auto dir = getDirection(preBounds, postBounds);
  auto bounds = getBounds(preBounds, postBounds, dir);
  const Vector3i range = slab(bounds);

  std::unordered_map<uint32_t, int> mappingCounts;
  std::unordered_map<uint32_t, int> sizes;

  zi::disjoint_sets<uint32_t> sets(range.x * range.y * range.z);
  std::set<uint32_t> included;

  chunk::VoxelGetter<uint32_t> preGetter(pre.ChunkDS(), pre.Coords());
  chunk::VoxelGetter<uint32_t> postGetter(post.ChunkDS(), pre.Coords());

  for (auto i = bounds.getMin().x + 1; i < bounds.getMax().x; i++) {
    for (auto j = bounds.getMin().y + 1; j < bounds.getMax().y; j++) {
      for (auto k = bounds.getMin().z + 1; k < bounds.getMax().z; k++) {
        const coords::Global g(i, j, k);
        uint32_t seg_id = preGetter.GetValue(g);
        uint32_t post_seg_id = postGetter.GetValue(g);

        sizes[post_seg_id]++;

        if (!selected.count(seg_id)) {
          continue;
        }

        mappingCounts[post_seg_id]++;

        uint32_t proxy = toProxy(g, range, bounds);
        assert(proxy ==
               toProxy(fromProxy(proxy, range, bounds), range, bounds));
        included.insert(proxy);

        const coords::Global g1(i - 1, j, k);
        uint32_t seg_id1 = preGetter.GetValue(g1);
        if (selected.count(seg_id1)) {
          auto p1 = toProxy(g1, range, bounds);
          included.insert(p1);
          conditionalJoin(sets, proxy, p1);
        }

        const coords::Global g2(i, j - 1, k);
        uint32_t seg_id2 = preGetter.GetValue(g2);
        if (selected.count(seg_id2)) {
          auto p2 = toProxy(g2, range, bounds);
          included.insert(p2);
          conditionalJoin(sets, proxy, p2);
        }

        const coords::Global g3(i, j, k - 1);
        uint32_t seg_id3 = preGetter.GetValue(g3);
        if (selected.count(seg_id3)) {
          auto p3 = toProxy(g3, range, bounds);
          included.insert(p3);
          conditionalJoin(sets, proxy, p3);
        }
      }
    }
  }

  std::unordered_map<uint32_t, std::set<uint32_t>> newSeedSets;
  std::unordered_map<uint32_t, std::set<uint32_t>> preSideSets;
  std::unordered_map<uint32_t, bool> escapes;
  for (auto& i : included) {
    escapes[sets.find_set(i)] = false;
  }
  for (auto& i : included) {
    const auto g = fromProxy(i, range, bounds);
    const auto root = sets.find_set(i);
    newSeedSets[root].insert(postGetter.GetValue(g));
    preSideSets[root].insert(preGetter.GetValue(g));
    if (!escapes[root] && inCriticalRegion(g, bounds, dir)) {
      escapes[root] = true;
    }
  }

  for (auto& seed : newSeedSets) {
    if (escapes[seed.first]) {
      seeds.push_back(makeSeed(seed.second, mappingCounts, sizes));
      log_debugs(unknown) << "Spawned!";
      std::stringstream ss;
      for (auto& seg : preSideSets[seed.first]) {
        ss << seg << ",";
      }
      log_debugs(unknown) << "Pre Side: " << ss.str();

      for (auto& seg : seeds.back()) {
        ss << seg.first << ",";
      }
      log_debugs(unknown) << "Post Side: " << ss.str();
    }
  }
}
}
}  // namespace om::handler
