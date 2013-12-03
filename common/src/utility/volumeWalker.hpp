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
      const std::set<T>& included,
      std::function<void(const coords::Data&, T value)> func) {

    auto wrappedFunc = [&included, func](const coords::Data& coord, T value) {
      if (included.count(value)) {
        func(coord, value);
      }
    };

    if (uniqueVals_ == nullptr) {
      foreach_voxel_internal(bounds_, wrappedFunc);
    }

    coords::Chunk currChunk = bounds_.getMin().ToChunk();
    coords::Chunk maxChunk = bounds_.getMax().ToChunk();

    for (currChunk.z = bounds_.getMin().z; currChunk.z < maxChunk.z;
         ++currChunk.z) {
      for (currChunk.y = bounds_.getMin().y; currChunk.y < maxChunk.y;
           ++currChunk.y) {
        for (currChunk.x = bounds_.getMin().x; currChunk.x < maxChunk.x;
             ++currChunk.x) {

          auto vals = uniqueVals_->Get(currChunk);
          if ((bool)vals && contains(*vals, included)) {
            foreach_voxel_internal(currChunk.BoundingBox(bounds_.volume()),
                                   wrappedFunc);
          }
        }
      }
    }
  }

 private:
  bool contains(const chunk::UniqueValues& uv, const std::set<T>& values) {
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
    log_variable(bounds);
    try {
      for (curr.z = bounds.getMin().z; curr.z < bounds.getMax().z; ++curr.z) {
        for (curr.y = bounds.getMin().y; curr.y < bounds.getMax().y; ++curr.y) {
          for (curr.x = bounds.getMin().x; curr.x < bounds.getMax().x;
               ++curr.x) {
            func(curr, voxels_.GetValue(curr));
          }
        }
      }
    }
    catch (Exception e) {
      log_debugs(Voxels) << curr << " " << e.what();
    }
  }

  const coords::DataBbox& bounds_;
  chunk::Voxels<T>& voxels_;
  chunk::UniqueValuesDS* uniqueVals_;
};
}
}  // namespace om::utility::