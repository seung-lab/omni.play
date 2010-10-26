#include "project/omProject.h"
#include "system/omProjectData.h"
#include "viewGroup/omViewGroupState.h"
#include "tiles/cache/omTileCache.h"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTileDumper.hpp"
#include "volume/omMipVolume.h"

OmTileDumper::OmTileDumper(const OmID volID, const ObjectType type,
						   const QString& dumpfile,
						   OmViewGroupState* vgs)
	: dumpfile_(dumpfile)
	, vgs_(vgs)
{
	if(SEGMENTATION == type) {
		mVolume = (OmMipVolume*)&OmProject::GetSegmentation(volID);
	} else if (CHANNEL == type) {
		mVolume = (OmMipVolume*)&OmProject::GetChannel(volID);
	} else {
		throw OmArgException("unknown object type");
	}
}

void OmTileDumper::DumpTiles()
{
	QFile file(dumpfile_);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);

	for(int mipLevel = 0; mipLevel <= mVolume->GetRootMipLevel(); ++mipLevel){

		//dim of miplevel in mipchunks
		const Vector3i mip_coord_dims =
			mVolume->MipLevelDimensionsInMipChunks(mipLevel);

		//for all coords
		for (int z = 0; z < mip_coord_dims.z; ++z){
			for (int y = 0; y < mip_coord_dims.y; ++y){
				for (int x = 0; x < mip_coord_dims.x; ++x){
					//TODO: fix viewType
					saveTile(out, mipLevel, x, y, z, XY_VIEW);
					saveTile(out, mipLevel, x, y, z, XZ_VIEW);
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
	const OmMipChunkCoord mipCoord(mipLevel, x, y, z);
	const DataBbox box = mVolume->MipCoordToDataBbox(mipCoord, mipLevel);
	const DataCoord data_coord = box.getMin();
	const SpaceCoord space_coord = mVolume->DataToSpaceCoord(data_coord);

	const int freshness = 0;

	const OmTileCoord tileCoord(mipLevel,
								space_coord,
								mVolume,
								freshness,
								vgs_,
								viewType,
								SEGMENTATION);

	OmTilePtr tile;
	OmProjectData::getTileCache()->doGet(tile, tileCoord, om::BLOCKING);

	const char* tileData = (const char*)tile->GetTexture()->getTileData();
	const int numBytes = tile->GetTexture()->NumBytes();

	out.writeBytes(tileData, numBytes);
}
