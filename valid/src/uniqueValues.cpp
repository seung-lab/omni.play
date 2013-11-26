#include "chunk/uniqueValuesFileDataSource.hpp"
#include "chunk/voxelGetter.hpp"
#include "utility/volumeWalker.hpp"
#include "uniqueValues.h"
#include "controller.hpp"

namespace om {
namespace valid {

bool UniqueValuesValid::Check() const {
  chunk::Voxels<uint32_t> vg(vol_.ChunkDS(), vol_.Coords());

  auto chunks = vol_.Coords().MipChunkCoords();
  {
    Controller::Context c("Chunks");

    VALID_FOR(i, 0, chunks->size(), 1) {
      coords::Chunk cc = (*chunks)[i];
      Controller::Context c(cc);

      auto uv = vol_.UniqueValuesDS().Get(cc);

      utility::VolumeWalker<uint32_t> walker(cc.BoundingBox(vol_.Coords()), vg);
      std::set<uint32_t> voxels;
      walker.foreach_voxel([&](coords::Data,
                               uint32_t val) { voxels.insert(val); });

      voxels.erase(0);

      VALID_TEST(voxels.size() == uv->Values.size(),
                 "Incorrect number of Unique Values");

      for (auto& v : voxels) {
        VALID_TEST(uv->contains(v),
                   std::string("Missing: ") + std::to_string(v));
      }
    }
  }
  return true;
}
}
}  // namespace om::valid::
