#include "project/omSegmentationManager.h"
#include "project/omProject.h"
#include "project/omProjectVolumes.h"
#include "viewGroup/omViewGroupState.h"
#include "tiles/cache/omTileCache.h"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTileDumper.hpp"
#include "volume/omMipVolume.h"
#include "project/omChannelManager.h"
#include "common/omDebug.h"

#include <QFile>
#include <QImage>
#include <QGLWidget>

OmTileDumper::OmTileDumper(const OmID volID, const ObjectType type,
						   const QString& dumpfile,
						   OmViewGroupState* vgs)
	: dumpfile_(dumpfile)
	, vgs_(vgs)
{
	if(SEGMENTATION == type) {
		mVolume = (OmMipVolume*)&OmProject::Volumes().Segmentations().GetSegmentation(volID);
	} else if (CHANNEL == type) {
		mVolume = (OmMipVolume*)&OmProject::Volumes().Channels().GetChannel(volID);
	} else {
		throw OmArgException("unknown object type");
	}
}

void OmTileDumper::DumpTiles()
{
	QFile file(dumpfile_);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);

	for(int mipLevel = 0; mipLevel <= mVolume->Coords().GetRootMipLevel(); ++mipLevel){

		//dim of miplevel in mipchunks
		const int chunkDim = mVolume->Coords().GetChunkDimension();
		const Vector3i mip_coord_dims =
			mVolume->Coords().MipLevelDimensionsInMipChunks(mipLevel) * chunkDim;

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
	const SpaceCoord space_coord = mVolume->Coords().DataToSpaceCoord(data_coord);

	const int freshness = 0;

	const OmTileCoord tileCoord(mipLevel,
								space_coord,
								mVolume,
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
	const GLint fmt = tile->GetTexture()->GetGLformat();
	QImage img;
	switch (fmt) {
		case GL_RGBA:
			img = QImage((const uchar*)tileData, tile->GetTexture()->GetWidth(), tile->GetTexture()->GetHeight(), QImage::Format_ARGB32);
		break;
			img = QImage((const uchar*)tileData, tile->GetTexture()->GetWidth(), tile->GetTexture()->GetHeight(), QImage::Format_Indexed8);
		default:
			assert(0 && "dumping unknown type");
		break;
	}

	QGLWidget::convertToGLFormat(img);
	img.save(dumpfile_ + QString("-%1-%2.%3.%4-%5.png").arg(mipLevel).arg(x).arg(y).arg(z).arg(viewType));


	out.writeBytes(tileData, numBytes);
}
