#include "chunk/voxelGetter.hpp"
#include "utility/volumeWalker.hpp"
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
  Vector3i min = bounds.getMin();
  return slab.x * slab.y * ((int)g.z - min.z) + slab.x * ((int)g.y - min.y) +
         (int)g.x - min.x;
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

  auto dims = bounds.getDimensions();

  if (dims.x < dims.y && dims.x < dims.z) {
    if (bounds.getMin().x > post.getMin().x) {
      return Direction::XMin;
    } else {
      return Direction::XMax;
    }
  }

  if (dims.y < dims.x && dims.x < dims.z) {
    if (bounds.getMin().y > post.getMin().y) {
      return Direction::YMin;
    } else {
      return Direction::YMax;
    }
  }

  if (bounds.getMin().z > post.getMin().z) {
    return Direction::ZMin;
  } else {
    return Direction::ZMax;
  }
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
  log_infos << "Getting Seeds\n" << pre.Endpoint() << '\n' << post.Endpoint();

  std::set<uint32_t> sel(selected.begin(), selected.end());

  auto preBounds = pre.Metadata().bounds();
  auto postBounds = post.Metadata().bounds();
  auto dir = getDirection(preBounds, postBounds);
  auto bounds = getBounds(preBounds, postBounds, dir);
  const Vector3i range = slab(bounds);

  std::unordered_map<uint32_t, int> mappingCounts;
  std::unordered_map<uint32_t, int> sizes;

  zi::disjoint_sets<uint32_t> sets(range.x * range.y * range.z);
  std::set<uint32_t> included, postSelected;

  chunk::Voxels<uint32_t> preGetter(pre.ChunkDS(), pre.Coords());
  chunk::Voxels<uint32_t> postGetter(post.ChunkDS(), pre.Coords());

  // Offset iteration bounds by 1 so we don't exceed the volume.
  auto iterBounds = bounds;
  iterBounds.setMin(bounds.getMin() + 1);

  utility::VolumeWalker<uint32_t> walker(iterBounds.ToDataBbox(pre.Coords(), 0),
                                         preGetter, &pre.UniqueValuesDS());

  walker.foreach_voxel_in_set(sel,
                              [&](const coords::Data& dc, uint32_t seg_id) {
    // if (!sel.count(seg_id)) {
    //   return;
    // }

    coords::Global g = dc.ToGlobal();
    uint32_t post_seg_id = postGetter.GetValue(dc);

    postSelected.insert(post_seg_id);
    mappingCounts[post_seg_id]++;

    uint32_t proxy = toProxy(g, range, bounds);
    included.insert(proxy);

    const coords::Global g1(g.x - 1, g.y, g.z);
    uint32_t seg_id1 = preGetter.GetValue(g1);
    if (sel.count(seg_id1)) {
      auto p1 = toProxy(g1, range, bounds);
      included.insert(p1);
      conditionalJoin(sets, proxy, p1);
    }

    const coords::Global g2(g.x, g.y - 1, g.z);
    uint32_t seg_id2 = preGetter.GetValue(g2);
    if (sel.count(seg_id2)) {
      auto p2 = toProxy(g2, range, bounds);
      included.insert(p2);
      conditionalJoin(sets, proxy, p2);
    }

    const coords::Global g3(g.x, g.y, g.z - 1);
    uint32_t seg_id3 = preGetter.GetValue(g3);
    if (sel.count(seg_id3)) {
      auto p3 = toProxy(g3, range, bounds);
      included.insert(p3);
      conditionalJoin(sets, proxy, p3);
    }
  });

  utility::VolumeWalker<uint32_t> postWalker(
      bounds.ToDataBbox(post.Coords(), 0), postGetter, &post.UniqueValuesDS());

  postWalker.foreach_voxel_in_set(
      postSelected,
      [&sizes](const coords::Data&, uint32_t seg_id) { sizes[seg_id]++; });

  std::unordered_map<uint32_t, common::SegIDSet> newSeedSets;
  std::unordered_map<uint32_t, common::SegIDSet> preSideSets;
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
      log_infos << "Spawned!";
      std::stringstream ss;
      for (auto& seg : preSideSets[seed.first]) {
        ss << seg << ",";
      }
      log_infos << "Pre Side: " << ss.str();

      for (auto& seg : seeds.back()) {
        ss << seg.first << ",";
      }
      log_infos << "Post Side: " << ss.str();
    }
  }
}
}
}  // namespace om::handler
