#pragma once

#include "utility/image.hpp"
#include "volume/volume.h"
#include "chunk/dataSources.hpp"
#include "volume/iterators.hpp"

namespace om {
namespace chunk {

class utils {
 public:
  static std::shared_ptr<UniqueValues> MakeUniqueValues(
      ChunkDS& ds, const coords::Chunk& cc,
      const coords::VolumeSystem& system) {
    common::SegIDSet voxels;
    auto iter =
        volume::make_all_dataval_iterator<uint32_t>(cc.BoundingBox(system), ds);
    auto end = volume::all_dataval_iterator<uint32_t>();
    for (; iter != end; ++iter) {
      if (iter->value()) {
        voxels.insert(iter->value());
      }
    }
    auto ret = std::make_shared<UniqueValues>(cc, voxels.begin(), voxels.end());
    std::sort(ret->Values.begin(), ret->Values.end());
    return ret;
  }
};
}
}  // namespace om::chunks