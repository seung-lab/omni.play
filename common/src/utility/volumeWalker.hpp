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

  void foreach_voxel(std::function<void(const coords::Data&, T)> func) {
    foreach_voxel_internal(bounds_, func);
  }

  void foreach_voxel_in_set(const std::set<T>& included,
                            std::function<void(const coords::Data&, T)> func) {
    auto wrappedFunc = [&included, func](const coords::Data& coord, T value) {
      if (included.count(value)) {
        func(coord, value);
      }
    };

    if (uniqueVals_ == nullptr) {
      foreach_voxel_internal(bounds_, wrappedFunc);
    }

    coords::Chunk minChunk = bounds_.getMin().ToChunk();
    coords::Chunk maxChunk = bounds_.getMax().ToChunk();
    auto currChunk = minChunk;

    for (currChunk.z = minChunk.z; currChunk.z <= maxChunk.z; ++currChunk.z) {
      for (currChunk.y = minChunk.y; currChunk.y <= maxChunk.y; ++currChunk.y) {
        for (currChunk.x = minChunk.x; currChunk.x <= maxChunk.x;
             ++currChunk.x) {
          auto vals = uniqueVals_->Get(currChunk);

          if ((bool)vals && contains(*vals, included)) {
            auto chunkbb = currChunk.BoundingBox(bounds_.volume());
            chunkbb.intersect(bounds_);
            foreach_voxel_internal(chunkbb, wrappedFunc);
          }
        }
      }
    }
  }

  bool true_foreach_voxel(
      std::function<bool(const coords::Data&, T value)> func) {
    return true_foreach_voxel_internal(bounds_, func);
  }

  bool true_foreach_voxel_in_set(
      const std::set<T>& included,
      std::function<bool(const coords::Data&, T value)> func) {
    auto wrappedFunc = [&included, func](const coords::Data& coord, T value) {
      // Short circuiting should skip the funtion call if it's not included.
      return !included.count(value) || func(coord, value);
    };

    if (uniqueVals_ == nullptr) {
      return true_foreach_voxel_internal(bounds_, wrappedFunc);
    }

    coords::Chunk minChunk = bounds_.getMin().ToChunk();
    coords::Chunk maxChunk = bounds_.getMax().ToChunk();
    auto currChunk = minChunk;

    for (currChunk.z = minChunk.z; currChunk.z <= maxChunk.z; ++currChunk.z) {
      for (currChunk.y = minChunk.y; currChunk.y <= maxChunk.y; ++currChunk.y) {
        for (currChunk.x = minChunk.x; currChunk.x <= maxChunk.x;
             ++currChunk.x) {
          auto vals = uniqueVals_->Get(currChunk);

          if ((bool)vals && contains(*vals, included)) {
            auto chunkbb = currChunk.BoundingBox(bounds_.volume());
            chunkbb.intersect(bounds_);
            if (!true_foreach_voxel_internal(chunkbb, wrappedFunc)) {
              return false;
            }
          }
        }
      }
    }
    return true;
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
      log_debugs << curr << " " << e.what();
    }
  }

  bool true_foreach_voxel_internal(
      const coords::DataBbox& bounds,
      std::function<bool(const coords::Data&, T value)> func) {
    coords::Data curr = bounds.getMin();
    try {
      for (curr.z = bounds.getMin().z; curr.z < bounds.getMax().z; ++curr.z) {
        for (curr.y = bounds.getMin().y; curr.y < bounds.getMax().y; ++curr.y) {
          for (curr.x = bounds.getMin().x; curr.x < bounds.getMax().x;
               ++curr.x) {
            if (!func(curr, voxels_.GetValue(curr))) {
              return false;
            }
          }
        }
      }
    }
    catch (Exception e) {
      log_debugs << curr << " " << e.what();
      return false;
    }
    return true;
  }

  const coords::DataBbox& bounds_;
  chunk::Voxels<T>& voxels_;
  chunk::UniqueValuesDS* uniqueVals_;
};
}
}  // namespace om::utility::