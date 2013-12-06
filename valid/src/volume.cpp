#include "chunk/cachedDataSource.hpp"
#include "volume.h"
#include "controller.hpp"

namespace om {
namespace valid {

class checkChunk : public boost::static_visitor<bool> {
 public:
  checkChunk(const coords::VolumeSystem& system, std::string prefix)
      : system_(system), prefix_(prefix) {}

  template <typename T>
  bool operator()(const chunk::Chunk<T>& chunk) const {
    auto bounds = chunk.coord().BoundingBox(system_);
    auto volBounds =
        system_.Extent().ToDataBbox(system_, chunk.coord().mipLevel());
    bounds.intersect(volBounds);

    std::map<T, int> counts;

    for (int x = bounds.getMin().x; x <= bounds.getMax().x; ++x) {
      for (int y = bounds.getMin().y; y <= bounds.getMax().y; ++y) {
        for (int z = bounds.getMin().z; z <= bounds.getMax().z; ++z) {
          coords::Data d(x, y, z, system_, chunk.coord().mipLevel());
          counts[chunk[d.ToChunkOffset()]]++;
        }
      }
    }
    log_infos << prefix_ << "Different Values Found: " << counts.size()
              << std::endl;
    log_infos << prefix_ << "Zero Values Found: " << counts[0] << std::endl;

    return counts.size() > 1;
  }

 private:
  const coords::VolumeSystem& system_;
  std::string prefix_;
};

bool VolumeValid::Check() const {
  auto& chunkDS = vol_.ChunkDS();

  auto chunks = vol_.Coords().MipChunkCoords();
  for (auto& cc : *chunks) {
    Controller::Context c(cc);
    auto chunk = chunkDS.Get(cc);

    VALID_TEST((bool)chunk, "Does Not Exist!");
    VALID_TEST(
        boost::apply_visitor(checkChunk(vol_.Coords(), prefix_ + "\t"), *chunk),
        "Bad Chunk");
  }
  return true;
}
}
}  // namespace om::valid::
