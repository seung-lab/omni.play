#ifndef OM_CACHING_TILE_H
#define OM_CACHING_TILE_H

#include "omTile.h"
#include "omTextureID.h"
#include "system/omThreadedCache.h"
#include "system/omEventManager.h"
#include "system/omCacheManager.h"
#include "system/events/omViewEvent.h"

#include "common/omStd.h"
#include "omTileCoord.h"

#include "volume/omMipVolume.h"

class OmCachingThreadedCachingTile;
class OmViewGroupState;

typedef OmThreadedCache< OmTileCoord, OmTextureID > TextureIDThreadedCache;

class QGLContext;


/**
 *	A Tile extended to support caching of TextureIDs with the GenericCache interface.
 *
 *	Rachel Shearer - rshearer@mit.edu
 */

class OmThreadedCachingTile 
: public OmTile, public TextureIDThreadedCache {
	
public:	
	OmThreadedCachingTile(ViewType, ObjectType, OmId, OmMipVolume *, const QGLContext *, OmViewGroupState * );
	
	~OmThreadedCachingTile();
		
	// texture ID
	virtual void GetTextureID(QExplicitlySharedDataPointer<OmTextureID> &p_value, const OmTileCoord &tileCoord, bool block = true);
	void GetTextureIDDownMip(QExplicitlySharedDataPointer<OmTextureID> &p_value, const OmTileCoord &tileCoord, int rootLevel, OmTileCoord &retCoord);
	void StoreTextureID(const OmTileCoord &tileCoord, OmTextureID* texID);
	
	//cache actions
	void Remove(const OmTileCoord &tileCoord);

	//void ClearCache();
	void SetContinuousUpdate(bool);
	
	ObjectType mVolType;
	OmId mImageId;

private:
	OmTextureID* HandleCacheMiss(const OmTileCoord &key);
	void HandleFetchUpdate();
	bool InitializeFetchThread();
	
	const QGLContext* mShareContext;
};



#endif

