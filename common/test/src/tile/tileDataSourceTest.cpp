#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "tile/cachedDataSource.hpp"
#include "utility/UUID.hpp"
#include "volume/volume.h"

using namespace om::tile;

namespace om {
namespace test {

#define URI "test/data/test.omni.files/"

TEST(Tile_FileDataSource, GetTile) {
  volume::Volume chan(std::string(URI) + "channels/channel1");
  volume::Volume seg(std::string(URI) + "segmentations/segmentation1");
  auto& chanSource = chan.TileDS();
  auto& segSource = seg.TileDS();

  chanSource.Get(
      coords::Tile(coords::Chunk(0, Vector3i(1)), common::XY_VIEW, 12));
  chanSource.Get(
      coords::Tile(coords::Chunk(0, Vector3i(1)), common::XZ_VIEW, 12));
  chanSource.Get(
      coords::Tile(coords::Chunk(0, Vector3i(1)), common::ZY_VIEW, 12));
  chanSource.Get(
      coords::Tile(coords::Chunk(1, Vector3i(0)), common::XY_VIEW, 100));

  segSource.Get(
      coords::Tile(coords::Chunk(0, Vector3i(1)), common::XY_VIEW, 12));
  segSource.Get(
      coords::Tile(coords::Chunk(0, Vector3i(1)), common::XZ_VIEW, 12));
  segSource.Get(
      coords::Tile(coords::Chunk(0, Vector3i(1)), common::ZY_VIEW, 12));
  segSource.Get(
      coords::Tile(coords::Chunk(1, Vector3i(0)), common::XY_VIEW, 100));
}

TEST(Tile_FileDataSource, PutTile) {
  // volume::Volume seg(std::string(URI) + "segmentations/segmentation1");
  // auto segSource = seg.TileDS();

  // coords::Tile coord(coords::Chunk(0, Vector3i(1)), common::XY_VIEW, 100);

  // tile::TileVar tile = tile::Tile<uint32_t>(coord, seg.coordSystem());

  // TODO: Set data

  // Not yet implemented
  // segSource.Put(coord, tile);

  // tile::TileVar gotten = segSource.Get(coord);

  // TODO: Compare results
}

#undef URI
}
}  // namespace om::test::
