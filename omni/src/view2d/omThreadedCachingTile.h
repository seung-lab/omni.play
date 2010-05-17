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


typedef OmThreadedCache< OmTileCoord, OmTextureID > TextureIDThreadedCache;

class QGLContext;


/**
 *	A Tile extended to support caching of TextureIDs with the GenericCache interface.
 *
 *	Rachel Shearer - rshearer@mit.edu
 */

class OmThreadedCachingTile : public OmTile, public TextureIDThreadedCache {
	
public:	
	OmThreadedCachingTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume *vol, const QGLContext *shareContext );
	
	~OmThreadedCachingTile();
		
	// texture ID
	virtual void GetTextureID(QExplicitlySharedDataPointer<OmTextureID> &p_value, const OmTileCoord &tileCoord, bool block = true);
	void GetTextureIDDownMip(QExplicitlySharedDataPointer<OmTextureID> &p_value, const OmTileCoord &tileCoord, int rootLevel, OmTileCoord &retCoord);
	void StoreTextureID(const OmTileCoord &tileCoord, OmTextureID* texID);
	
	//cache actions
	void Remove(const OmTileCoord &tileCoord);

	//void ClearCache();
	void SetContinuousUpdate(bool);
	
	void subImageTex(set< DataCoord > &vox );
	
	ObjectType mVolType;
	OmId mImageId;

private:
	OmTextureID* HandleCacheMiss(const OmTileCoord &key);
	void HandleFetchUpdate();
	bool InitializeFetchThread();
	
	const QGLContext* mShareContext;
};


class OmCachingThreadedCachingTile
{
public:
	OmCachingThreadedCachingTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume *vol, const QGLContext *shareContext)
	{

		static vector<OmCachingThreadedCachingTile*> caches;

		if(NULL == vol) {
			mDelete = true;
			return;
		}

		mViewtype = viewtype;
		mVoltype = voltype;
		mImage_id = image_id;
		mVol = vol;
		mCache = NULL;
		mDelete = false;

		for(unsigned int i = 0; i < caches.size(); i++) {
			if (caches[i]->mViewtype == mViewtype	&&
			    caches[i]->mVoltype == mVoltype 	&&
			    caches[i]->mImage_id == mImage_id	&&
			    caches[i]->mVol == mVol) {
				mCache = caches[i]->mCache;
				mDelete = true;
			}
		}

		if(!mCache) {
			mCache = new OmThreadedCachingTile (viewtype, voltype, image_id, vol, shareContext);
			caches.push_back (this);
		}


	}

	static unsigned int Freshen(bool freshen) {
		return OmCacheManager::Freshen(freshen);
	}

	static void Refresh() {
		Freshen (true);
		OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
	}

	bool mDelete;
	OmThreadedCachingTile * mCache;

private:
	ViewType mViewtype;
	ObjectType mVoltype;
	OmId mImage_id;
	OmMipVolume *mVol;

};

#endif

