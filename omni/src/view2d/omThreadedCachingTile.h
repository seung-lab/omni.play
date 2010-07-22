#ifndef OM_CACHING_TILE_H
#define OM_CACHING_TILE_H

#include "omTile.h"
#include "omTextureID.h"
#include "system/cache/omThreadedCache.h"
#include "system/omEventManager.h"
#include "system/cache/omCacheManager.h"
#include "system/events/omViewEvent.h"
#include "common/omStd.h"
#include "omTileCoord.h"
#include "volume/omMipVolume.h"

class OmTileCache;
class OmViewGroupState;

class QGLContext;


/**
 *	A Tile extended to support caching of TextureIDs with the GenericCache interface.
 *
 *	Rachel Shearer - rshearer@mit.edu
 */

class OmThreadedCachingTile : public OmTile {
	
public:	
	OmThreadedCachingTile(ViewType, ObjectType, OmId, OmMipVolume *, const QGLContext *, OmViewGroupState * );
	
	~OmThreadedCachingTile();
		
	// texture ID
	virtual void GetTextureID(QExplicitlySharedDataPointer<OmTextureID> &p_value, const OmTileCoord &tileCoord, bool block = true);
	void GetTextureIDDownMip(QExplicitlySharedDataPointer<OmTextureID> &p_value, const OmTileCoord &tileCoord, int rootLevel, OmTileCoord &retCoord);

	ObjectType mVolType;
	OmId mImageId;

private:
	OmTileCache *const mDataCache;
	
	const QGLContext* mShareContext;
};



#endif

