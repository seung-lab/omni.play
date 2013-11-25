#pragma once

#include "chunk/voxelGetter.hpp"

namespace om {
namespace utility {

template <typename T>
class VolumeWalker {
 public:
  VolumeWalker(const coords::DataBbox& bounds, chunk::Voxels<T>& voxels,
               chunk::UniqueValuesDS* uniqueVals = nullptr)
      : bounds_(bounds), voxels_(voxels), uniqueVals_(uniqueVals) {}

  void foreach_voxel(std::function<void(const coords::Data&, T value)> func) {
    foreach_voxel_internal(bounds_, func);
  }

  void foreach_voxel_in_set(
      const std::set<T>& toIter,
      std::function<void(const coords::Data&, T value)> func) {

    if (uniqueVals_ == nullptr) {
      foreach_voxel_internal(bounds_, func);
    }

    coords::Chunk currChunk = bounds_.getMin().ToChunk();
    coords::Chunk maxChunk = bounds_.getMax().ToChunk();

    for (; currChunk.x < maxChunk.x; ++currChunk.x) {
      for (; currChunk.y < maxChunk.y; ++currChunk.y) {
        for (; currChunk.z < maxChunk.z; ++currChunk.z) {

          auto vals = uniqueVals_->Get(currChunk);
          if (vals && contains_one(*vals, toIter)) {
            foreach_voxel_internal(currChunk.BoundingBox(bounds_.volume()),
                                   func);
          }
        }
        currChunk.z = bounds_.getMin().z;
      }
      currChunk.y = bounds_.getMin().y;
    }
  }

 private:
  bool contains_one(const chunk::UniqueValues& uv, const std::set<T>& values) {
    for (auto& v : values) {
      if (uv.contains(v)) {
        return true;
      }
    }
    return false;
  }

  void foreach_voxel_internal(
      const coords::DataBbox& bounds,
      std::function<void(const coords::Data&, T value)> func) {
    coords::Data curr = bounds.getMin();
    for (; curr.x < bounds.getMax().x; ++curr.x) {
      for (; curr.y < bounds.getMax().y; ++curr.y) {
        for (; curr.z < bounds.getMax().z; ++curr.z) {
          func(curr, voxels_.GetValue(curr));
        }
        curr.z = bounds.getMin().z;
      }
      curr.y = bounds.getMin().y;
    }
  }

  const coords::DataBbox& bounds_;
  chunk::Voxels<T>& voxels_;
  chunk::UniqueValuesDS* uniqueVals_;
};
}
}  // namespace om::utility::