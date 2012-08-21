#include "common/omDebug.h"
#include "tiles/cache/omTileCache.h"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTileDumper.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omMipVolume.h"

#include <QFile>
#include <QImage>
#include <QGLWidget>

OmTileDumper::OmTileDumper(OmMipVolume* vol, const QString& dumpfile,
                           OmViewGroupState* vgs)
    : vol_(vol)
    , dumpfile_(dumpfile)
    , vgs_(vgs)
{}

void OmTileDumper::DumpTiles()
{
    QFile file(dumpfile_);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    for(int mipLevel = 0; mipLevel <= vol_->Coords().GetRootMipLevel(); ++mipLevel){

        //dim of miplevel in mipchunks
        const int chunkDim = vol_->Coords().GetChunkDimension();
        const Vector3i mip_coord_dims =
            vol_->Coords().MipLevelDimensionsInMipChunks(mipLevel) * chunkDim;

        //printf("dims: %i,%i,%i\n", mip_coord_dims.x, mip_coord_dims.y, mip_coord_dims.z);

        //for all coords
        for (int z = 0; z < mip_coord_dims.z; ++z){
            for (int y = 0; y < mip_coord_dims.y; y+=chunkDim){
                for (int x = 0; x < mip_coord_dims.x; x+=chunkDim){
                    saveTile(out, mipLevel, x, y, z, XY_VIEW);
                }
            }
        }
        for (int z = 0; z < mip_coord_dims.z; z+=chunkDim){
            for (int y = 0; y < mip_coord_dims.y; ++y){
                for (int x = 0; x < mip_coord_dims.x; x+=chunkDim){
                    saveTile(out, mipLevel, x, y, z, XZ_VIEW);
                }
            }
        }
        for (int z = 0; z < mip_coord_dims.z; z+=chunkDim){
            for (int y = 0; y < mip_coord_dims.y; y+=chunkDim){
                for (int x = 0; x < mip_coord_dims.x; ++x){
                    saveTile(out, mipLevel, x, y, z, ZY_VIEW);
                }
            }
        }

    }
}

void OmTileDumper::saveTile(QDataStream& out, const int mipLevel,
                            const int x, const int y, const int z,
                            const ViewType viewType)
{
    const om::globalCoord coord = om::globalCoord(x, y, z);
    const om::chunkCoord chunk = coord.toChunkCoord(vol_, mipLevel);
    const om::dataCoord data = coord.toDataCoord(vol_, mipLevel);
    const int freshness = 0;

    const OmTileCoord tileCoord(chunk,
                                viewType,
                                data.toTileOffset(viewType),
                                vol_,
                                freshness,
                                vgs_,
                                SEGMENTATION);

    OmTilePtr tile;
    OmTileCache::Get(tile, tileCoord, om::BLOCKING);

    const OmTextureID& texture = tile->GetTexture();
    const int numBytes = tile->NumBytes();

    const char* tileData = (const char*)texture.GetTileData();
    const QImage::Format qimageFormat = texture.GetQTimageFormat();

    QImage img = QImage((const uchar*)tileData,
                        texture.GetWidth(),
                        texture.GetHeight(),
                        qimageFormat);

    QGLWidget::convertToGLFormat(img);

    img.save(dumpfile_ + QString("-%1-%2.%3.%4-%5.png").arg(mipLevel).arg(x).arg(y).arg(z).arg(viewType));

    out.writeBytes(tileData, numBytes);
}
