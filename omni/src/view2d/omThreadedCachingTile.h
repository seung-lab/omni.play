#ifndef OM_CACHING_TILE_H
#define OM_CACHING_TILE_H

#include "omTileCoord.h"
#include "omTile.h"
#include "omTextureID.h"
#include "system/cache/omThreadedCache.h"
#include "system/omEventManager.h"
#include "system/cache/omCacheManager.h"
#include "system/events/omViewEvent.h"
#include "common/omStd.h"
#include "volume/omMipVolume.h"

class OmTileCache;
class OmTileCoord;
class OmViewGroupState;

class QGLContext;


/**
 *	A Tile extended to support caching of TextureIDs
 *
 *	Rachel Shearer - rshearer@mit.edu
 */

class OmThreadedCachingTile : public OmTile {
	
public:	
	OmThreadedCachingTile(ViewType, ObjectType, OmId, OmMipVolume *, const QGLContext *, OmViewGroupState * );
	
	~OmThreadedCachingTile();
		
	// texture ID
	virtual void GetTextureID(OmTextureIDPtr& p_value, const OmTileCoord &tileCoord, bool block = true);
	void GetTextureIDDownMip(OmTextureIDPtr& p_value, const OmTileCoord &tileCoord, int rootLevel, OmTileCoord &retCoord);

	ObjectType mVolType;
	OmId mImageId;

	void Remove(const OmTileCoord & coord);

private:
	OmTileCache *const mDataCache;
	
	const QGLContext* mShareContext;
};



#endif

