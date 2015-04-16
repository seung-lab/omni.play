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

std::ostream& operator<<(std::ostream& out, const Direction& d) {
  switch (d) {
    case Direction::XMin:
      return out << "XMin";
    case Direction::XMax:
      return out << "XMax";
    case Direction::YMin:
      return out << "YMin";
    case Direction::YMax:
      return out << "YMax";
    case Direction::ZMin:
      return out << "ZMin";
    case Direction::ZMax:
      return out << "ZMax";
  }
  return out;
}

inline Vector3i slab(const coords::GlobalBbox& bounds) {
  return Vector3i(bounds.getMax().x - bounds.getMin().x,
                  bounds.getMax().y - bounds.getMin().y,
                  bounds.getMax().z - bounds.getMin().z) +
         Vector3i::ONE;
}

inline uint32_t toProxy(const coords::Data& g, const Vector3i& slab,
                        const coords::DataBbox& bounds) {
  auto min = bounds.getMin();
  return slab.x * slab.y * (g.z - min.z) + slab.x * (g.y - min.y) + g.x - min.x;
}

inline coords::Data fromProxy(const uint32_t proxy, const Vector3i& slab,
                              const coords::DataBbox& bounds) {
  auto min = bounds.getMin();
  return coords::Data(proxy % slab.x + min.x,
                      (proxy % (slab.x * slab.y)) / slab.x + min.y,
                      proxy / (slab.x * slab.y) + min.z, bounds.volume(), 0);
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

  if (dims.y < dims.x && dims.y < dims.z) {
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

inline bool inCriticalRegion(const coords::Data& g,
                             const coords::DataBbox& bounds,
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
    if (seg == 0) {
      continue;
    }
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
                             const coords::GlobalBbox& post, const Direction d,
                             const Vector3i& resolution) {
  auto bounds = pre;
  bounds.intersect(post);

  // Trim on preside
  const int FUDGE = 5;
  auto min = bounds.getMin();
  auto max = bounds.getMax();

  switch (d) {
    case Direction::XMin:
      bounds.setMax(Vector3f(max.x - FUDGE * resolution.x, max.y, max.z));
      break;
    case Direction::YMin:
      bounds.setMax(Vector3f(max.x, max.y - FUDGE * resolution.y, max.z));
      break;
    case Direction::ZMin:
      bounds.setMax(Vector3f(max.x, max.y, max.z - FUDGE * resolution.z));
      break;
    case Direction::XMax:
      bounds.setMin(Vector3f(min.x + FUDGE * resolution.x, min.y, min.z));
      break;
    case Direction::YMax:
      bounds.setMin(Vector3f(min.x, min.y + FUDGE * resolution.y, min.z));
      break;
    case Direction::ZMax:
      bounds.setMin(Vector3f(min.x, min.y, min.z + FUDGE * resolution.z));
      break;
  }

  bounds.intersect(bounds);
  // So that bounds.isEmpty() returns the correct result. The implementation of
  // isEmpty() relies on an _empty data member that is not updated until
  // intersect() is called.
  return bounds;
}

// get the seeds in an adjacent volume given selected segments in a given volume
void get_seeds(std::vector<std::map<int32_t, int32_t>>& seeds,
               const volume::Segmentation& pre,
               const std::set<int32_t>& selected,
               const volume::Segmentation& post) {
  log_infos << "Getting Seeds\n" << pre.Endpoint() << '\n' << post.Endpoint();

  std::set<uint32_t> sel(selected.begin(), selected.end());

  auto preBounds = pre.Metadata().bounds();
  auto postBounds = post.Metadata().bounds();
  auto dir = getDirection(preBounds, postBounds);
  auto res = pre.Coords().Resolution();
  auto bounds = getBounds(preBounds, postBounds, dir, res);
  if (bounds.isEmpty()) {
    log_errors << "get_seeds: Bounds do not overlap: \n" << pre.Endpoint()
               << '\n' << post.Endpoint();
    throw ArgException("Adjusted bounds do not overlap.");
  }

  // Only have to work in the region where there are segments we care about.
  coords::DataBbox segBounds(pre.Coords(), 0);
  auto& segData = pre.SegData();
  for (auto& segID : selected) {
    if (segID < segData.size() && segID > 0) {
      segBounds.merge(segData[segID].bounds);
    }
  }

  bounds.intersect(segBounds.ToGlobalBbox());
  if (bounds.isEmpty()) {
    log_debugs << "No segments in non-fudge area.";
    return;
  }

  auto proxyBounds = bounds.ToDataBbox(pre.Coords(), 0);
  const Vector3i range = slab(proxyBounds);
  assert(range.x >= 0 && range.y >= 0 && range.z >= 0);
  uint64_t overlap_volume =
      (uint64_t)range.x * (uint64_t)range.y * (uint64_t)range.z;
  if (overlap_volume > 400 * 128 * 128 * 128) {  // Max overlap size
    log_errors << "get_seeds: Overlap region is too large: \n" << pre.Endpoint()
               << '\n' << post.Endpoint();
    throw ArgException("Overlap region is too large.");
  }

  std::unordered_map<uint32_t, int> mappingCounts;
  std::unordered_map<uint32_t, int> sizes;

  zi::disjoint_sets<uint32_t> sets(overlap_volume);
  std::set<uint32_t> included, postSelected;

  chunk::Voxels<uint32_t> preGetter(pre.ChunkDS(), pre.Coords());
  chunk::Voxels<uint32_t> postGetter(post.ChunkDS(), post.Coords());

  // Offset iteration bounds by 1 so we don't exceed the volume.
  auto iterBounds = bounds.ToDataBbox(pre.Coords(), 0);
  iterBounds.setMin(iterBounds.getMin() + 1);

  for (auto iter : pre.SegIterate(sel, iterBounds)) {
    uint32_t post_seg_id = postGetter.GetValue(iter.coord().ToGlobal());

    if (post_seg_id) {
      postSelected.insert(post_seg_id);
      mappingCounts[post_seg_id]++;
    }

    uint32_t proxy = toProxy(iter.coord(), range, proxyBounds);
    included.insert(proxy);

    auto considerNeighbor = [&](Vector3i offset) {
      auto n = iter.neighbor(offset);
      if ((bool)n && sel.count(n->value())) {
        auto p = toProxy(n->coord(), range, proxyBounds);
        sets.join(sets.find_set(proxy), sets.find_set(p));
      }
    };
    considerNeighbor(Vector3i(-1, 0, 0));
    considerNeighbor(Vector3i(0, -1, 0));
    considerNeighbor(Vector3i(0, 0, -1));
  }

  // Note sizes here are potentially counting one more slice in any direction
  // than the mappingCounts above due to the offset-by-1 iterBounds, but this
  // can actually help us get rid of mis-inclusions of segments overlapping only
  // on the last (few) slice(s).
  for (auto& iter : post.SegIterate(postSelected, bounds)) {
    sizes[iter.value()]++;
  }

  std::unordered_map<uint32_t, common::SegIDSet> newSeedSets;
  std::unordered_map<uint32_t, common::SegIDSet> preSideSets;
  std::unordered_map<uint32_t, bool> escapes;
  for (auto& i : included) {
    escapes[sets.find_set(i)] = false;
  }
  for (auto& i : included) {
    const auto dc = fromProxy(i, range, proxyBounds);
    const auto root = sets.find_set(i);
    newSeedSets[root].insert(postGetter.GetValue(dc.ToGlobal()));
    preSideSets[root].insert(preGetter.GetValue(dc));
    if (!escapes[root] &&
        inCriticalRegion(dc, bounds.ToDataBbox(pre.Coords(), 0), dir)) {
      escapes[root] = true;
    }
  }

  for (auto& seed : newSeedSets) {
    if (escapes[seed.first]) {
      seeds.push_back(makeSeed(seed.second, mappingCounts, sizes));
      log_infos << "Spawned!";
      log_infos << "Pre Side: " << om::string::join(preSideSets[seed.first]);

      std::stringstream ss;
      for (auto& seg : seeds.back()) {
        ss << seg.first << ", ";
      }
      log_infos << "Post Side: " << ss.str();
    } else {
      log_debugs << "Does not escape: "
                 << om::string::join(preSideSets[seed.first]);
    }
  }
}
}
}  // namespace om::handler
