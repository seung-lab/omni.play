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
                    saveTile(out, mipLevel, x, y, z, YZ_VIEW);
                }
            }
        }

    }
}

void OmTileDumper::saveTile(QDataStream& out, const int mipLevel,
                            const int x, const int y, const int z,
                            const ViewType viewType)
{

    const DataCoord data_coord = DataCoord(x, y, z);

    const int freshness = 0;

    const OmTileCoord tileCoord(mipLevel,
                                data_coord,
                                vol_,
                                freshness,
                                vgs_,
                                viewType,
                                SEGMENTATION);
    //printf("%i %i %i %i %i + ", mipLevel, x, y, z, viewType);
    //printf("%f %f %f\n", DEBUGV3(space_coord));


    OmTilePtr tile;

    OmTileCache::doGet(tile, tileCoord, om::BLOCKING);

    const char* tileData = (const char*)tile->GetTexture()->GetTileData();
    const int numBytes = tile->GetTexture()->NumBytes();
    const QImage::Format qimageFormat = tile->GetTexture()->GetQTimageFormat();

    QImage img = QImage((const uchar*)tileData,
                        tile->GetTexture()->GetWidth(),
                        tile->GetTexture()->GetHeight(),
                        qimageFormat);

    QGLWidget::convertToGLFormat(img);

    img.save(dumpfile_ + QString("-%1-%2.%3.%4-%5.png").arg(mipLevel).arg(x).arg(y).arg(z).arg(viewType));


    out.writeBytes(tileData, numBytes);
}
