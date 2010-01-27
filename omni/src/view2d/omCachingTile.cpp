
#include "omCachingTile.h"
#include "system/omDebug.h"

#define DEBUG 0

void OmCachingTile::GetTextureID(shared_ptr < OmTextureID > &p_value, const OmTileCoord & tileCoord)
{
	// //debug("genone","OmCachingTile::GetTextureID: " << tileCoord.get<0>() << " " << tileCoord.get<1>());

	TextureIDCache::Get(p_value, tileCoord);
	return;
}

void OmCachingTile::StoreTextureID(const OmTileCoord & tileCoord, OmTextureID * texID)
{
	TextureIDCache::Add(tileCoord, texID);
}

OmTextureID *OmCachingTile::HandleCacheMiss(const OmTileCoord & key)
{

	return BindToTextureID(key);
	// OmTile: OmTextureID* BindToTextureID(const OmTileCoordinate &key);
}

void OmCachingTile::ClearCache()
{
	TextureIDCache::Clear();
}

void OmCachingTile::setSecondMipVolume(ObjectType secondtype, OmId second_id, OmMipVolume * secondvol)
{
	isSecondMipVolume = true;

	AddOverlay(secondtype, second_id, secondvol);
}

void OmCachingTile::subImageTex(shared_ptr < OmTextureID > &texID, int dim, set < DataCoord > &vox, QColor & color,
				int tl)
{
	ReplaceTextureRegion(texID, dim, vox, color, tl);
}
