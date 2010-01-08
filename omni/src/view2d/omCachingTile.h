#ifndef OM_CACHING_TILE_H
#define OM_CACHING_TILE_H

/*
 *	A Tile extended to support caching of TextureIDs with the GenericCache interface.
 *
 *	Rachel Shearer - rshearer@mit.edu
 */

#include "omTile.h"
#include "omTextureID.h"
#include "system/omThreadedCache.h"
#include "volume/omVolumeTypes.h"

#include <QColor>

#include <boost/tuple/tuple_comparison.hpp>
using boost::tuple;

// typedef tuple<int, SpaceCoord> OmTileCoordinate;	
typedef OmThreadedCache< OmTileCoord, OmTextureID > TextureIDCache;


class OmCachingTile : public OmTile, protected TextureIDCache {
	
public:	
	OmCachingTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume *vol)
	: TextureIDCache(RAM_CACHE_GROUP), OmTile(viewtype, voltype, image_id, vol) {
		isSecondMipVolume = false;
	}
	
	// texture ID
	virtual shared_ptr<OmTextureID> GetTextureID(const OmTileCoord &tileCoord);
	void StoreTextureID(const OmTileCoord &tileCoord, OmTextureID* texID);
	
	//cache actions
	void ClearCache();
	
	// setting second OmMipVolume for overlay
	void setSecondMipVolume(ObjectType secondtype, OmId second_id, OmMipVolume *secondvol);
	bool checkSecondMipVolume() { return isSecondMipVolume; }
	
	void subImageTex(shared_ptr<OmTextureID> &texID, int dim, set< DataCoord > &vox, QColor &color, int tl);


private:
	OmTextureID* HandleCacheMiss(const OmTileCoord &key);
	bool isSecondMipVolume;
	
	
};


#endif
