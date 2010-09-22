#include "view2d/omTileDumper.hpp"
#include "system/viewGroup/omViewGroupState.h"

void OmTileDumper::DumpTiles(OmId, ObjectType, const QString, OmViewGroupState *)
{
	throw OmIoException("not implemented");
/*
	quint32 bps = 0;
	QFile file(dumpfile);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);

	OmMipVolume * rMipVolume;
	OmThreadedCachingTile * cache;

	if(SEGMENTATION == type) {
		bps = 4;
		rMipVolume = &OmProject::GetSegmentation(vol);
		cache = (new OmCachingThreadedCachingTile (XY_VIEW, type, vol, &OmProject::GetSegmentation(vol), NULL, vgs))->mCache;
	} else if(CHANNEL == type) {
		bps = 1;
		rMipVolume = (OmMipVolume*) &OmProject::GetChannel(vol);
		cache = (new OmCachingThreadedCachingTile (XY_VIEW, type, vol, rMipVolume, NULL, vgs))->mCache;
	} else {
		printf("OmMipVolume::DumpTiles called incorrectly\n");
		return;
	}

        for (int level = 0; level <= rMipVolume->GetRootMipLevel(); ++level) {

                //dim of miplevel in mipchunks
                Vector3 < int > mip_coord_dims = rMipVolume->MipLevelDimensionsInMipChunks(level);

                //for all coords
                for (int z = 0; z < mip_coord_dims.z; ++z){
                        for (int y = 0; y < mip_coord_dims.y; ++y){
                                for (int x = 0; x < mip_coord_dims.x; ++x){

					QExplicitlySharedDataPointer < OmTextureID > gotten_id = QExplicitlySharedDataPointer < OmTextureID > ();

					DataBbox box = rMipVolume->MipCoordToDataBbox(OmMipChunkCoord(level, x, y, z), level);
                        		DataCoord this_data_coord = DataCoord(box.getMin().x, box.getMin().y, box.getMin().z);;
                        		SpaceCoord this_space_coord = rMipVolume->DataToSpaceCoord(this_data_coord);
                        		OmTileCoord mTileCoord = OmTileCoord(level, this_space_coord, type, 0);

                                	cache->GetTextureID(gotten_id, mTileCoord, true);

					if(!gotten_id) {
						continue;
					}

					out.writeBytes((const char*)gotten_id->texture, gotten_id->x * gotten_id->y * bps);
                                }
                        }
                }
        }
*/
}
