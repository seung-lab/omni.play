#include "common/logging.h"
#include "tiles/cache/omTileCache.h"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "coordinates/tile.h"
#include "tiles/omTileDumper.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omMipVolume.h"

OmTileDumper::OmTileDumper(OmMipVolume* vol, const QString& dumpfile,
                           OmViewGroupState& vgs)
    : vol_(vol), dumpfile_(dumpfile), vgs_(vgs) {}

void OmTileDumper::DumpTiles() {
  QFile file(dumpfile_);
  file.open(QIODevice::WriteOnly);
  QDataStream out(&file);

  for (int mipLevel = 0; mipLevel <= vol_->Coords().RootMipLevel();
       ++mipLevel) {

    // dim of miplevel in mipchunks
    const int chunkDim = vol_->Coords().ChunkDimensions().x;
    const Vector3i mip_coord_dims =
        vol_->Coords().MipLevelDimensionsInMipChunks(mipLevel) * chunkDim;

    // for all coords
    for (int z = 0; z < mip_coord_dims.z; ++z) {
      for (int y = 0; y < mip_coord_dims.y; y += chunkDim) {
        for (int x = 0; x < mip_coord_dims.x; x += chunkDim) {
          saveTile(out, mipLevel, x, y, z, om::common::XY_VIEW);
        }
      }
    }
    for (int z = 0; z < mip_coord_dims.z; z += chunkDim) {
      for (int y = 0; y < mip_coord_dims.y; ++y) {
        for (int x = 0; x < mip_coord_dims.x; x += chunkDim) {
          saveTile(out, mipLevel, x, y, z, om::common::XZ_VIEW);
        }
      }
    }
    for (int z = 0; z < mip_coord_dims.z; z += chunkDim) {
      for (int y = 0; y < mip_coord_dims.y; y += chunkDim) {
        for (int x = 0; x < mip_coord_dims.x; ++x) {
          saveTile(out, mipLevel, x, y, z, om::common::ZY_VIEW);
        }
      }
    }
  }
}

void OmTileDumper::saveTile(QDataStream& out, const int mipLevel, const int x,
                            const int y, const int z,
                            const om::common::ViewType viewType) {
  const om::coords::Global coord = om::coords::Global(x, y, z);
  const om::coords::Chunk chunk = coord.ToChunk(vol_->Coords(), mipLevel);
  const om::coords::Data data = coord.ToData(vol_->Coords(), mipLevel);
  const int freshness = 0;

  const OmTileCoord tileCoord(chunk, viewType, data.ToTileOffset(viewType),
                              *vol_, freshness, vgs_, om::common::SEGMENTATION);

  OmTilePtr tile;
  OmTileCache::Get(tile, tileCoord, om::common::Blocking::BLOCKING);

  const OmTextureID* texture = tile->GetTexture();
  if (!texture) {
    log_errors << "Unable to saveTile" << tileCoord;
    return;
  }
  const int numBytes = tile->NumBytes();

  const char* tileData = (const char*)texture->GetTileData();
  const QImage::Format qimageFormat = texture->GetQTimageFormat();

  QImage img = QImage((const uchar*)tileData, texture->GetWidth(),
                      texture->GetHeight(), qimageFormat);

  QGLWidget::convertToGLFormat(img);

  img.save(
      dumpfile_ +
      QString("-%1-%2.%3.%4-%5.png").arg(mipLevel).arg(x).arg(y).arg(z).arg(
          viewType));

  out.writeBytes(tileData, numBytes);
}
