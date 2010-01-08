
#include "omCachingTile.h"

#define DEBUG 0


shared_ptr<OmTextureID> OmCachingTile::GetTextureID(const OmTileCoord &tileCoord) {
	// DOUT("OmCachingTile::GetTextureID: " << tileCoord.get<0>() << " " << tileCoord.get<1>());
	
	return TextureIDCache::Get(tileCoord);
}

void OmCachingTile::StoreTextureID(const OmTileCoord &tileCoord, OmTextureID* texID) {
	TextureIDCache::Add(tileCoord, texID);
}

OmTextureID* OmCachingTile::HandleCacheMiss(const OmTileCoord &key) {
	DOUT("OmCachingTile::HandleCacheMiss: " << key.Level << " " << key.Coordinate);
	
	return BindToTextureID(key);
	// OmTile: OmTextureID* BindToTextureID(const OmTileCoordinate &key);
}


void OmCachingTile::ClearCache() { 
	TextureIDCache::Clear(); 
}

void OmCachingTile::setSecondMipVolume(ObjectType secondtype, OmId second_id, OmMipVolume *secondvol) {
	isSecondMipVolume = true;
	
	AddOverlay(secondtype, second_id, secondvol);
}

void OmCachingTile::subImageTex(shared_ptr<OmTextureID> &texID, int dim, set< DataCoord > &vox, QColor &color, int tl) {
	ReplaceTextureRegion(texID, dim, vox, color, tl);
}

