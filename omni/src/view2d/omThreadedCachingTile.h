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


#include "common/omStd.h"
#include "omTileCoord.h"

#include "volume/omMipVolume.h"

#include <boost/tuple/tuple_comparison.hpp>
using boost::tuple;


typedef OmThreadedCache< OmTileCoord, OmTextureID > TextureIDThreadedCache;

class QGLContext;


class OmThreadedCachingTile : public OmTile, public TextureIDThreadedCache {
	
public:	
	OmThreadedCachingTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume *vol, const QGLContext *shareContext );
	
	~OmThreadedCachingTile();
	
	
	// texture ID
	virtual shared_ptr<OmTextureID> GetTextureID(const OmTileCoord &tileCoord, bool block = true);
	shared_ptr<OmTextureID> GetTextureIDDownMip (const OmTileCoord &tileCoord, int rootLevel, OmTileCoord &retCoord);
	void StoreTextureID(const OmTileCoord &tileCoord, OmTextureID* texID);
	
	//cache actions
	void Remove(const OmTileCoord &tileCoord);
//	void ClearCache();
	void SetContinuousUpdate(bool);
	
	// setting second OmMipVolume for overlay
	void setSecondMipVolume(ObjectType secondtype, OmId second_id, OmMipVolume *secondvol);
	bool checkSecondMipVolume() { return isSecondMipVolume; }
	
	void subFullImageTex(shared_ptr<OmTextureID> &texID, DataCoord firstCoord, int tl);
	void subImageTex(shared_ptr<OmTextureID> &texID, int dim, set< DataCoord > &vox, QColor &color, int tl);
	
	void SetMaxCacheSize(int bytes);
	
	ObjectType mVolType;
	OmId mImageId;
private:
	OmTextureID* HandleCacheMiss(const OmTileCoord &key);
	void HandleFetchUpdate();
	bool InitializeFetchThread();
	
	bool isSecondMipVolume;
	
	QGLContext* mFetchThreadContext;
	const QGLContext* mShareContext;
};


class OmCachingThreadedCachingTile
{
public:
	OmCachingThreadedCachingTile (ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume *vol, const QGLContext *shareContext)
	{
		static vector<OmCachingThreadedCachingTile*> caches;

		if (NULL == vol) {
			caches.clear ();
			mDelete = true;
			return;
		}

		mViewtype = viewtype;
		mVoltype = voltype;
		mImage_id = image_id;
		mVol = vol;
		mCache = NULL;
		mDelete = false;

		for (int i = 0; i < caches.size(); i++) {
			if (caches[i]->mViewtype == mViewtype	&&
			    caches[i]->mVoltype == mVoltype 	&&
			    caches[i]->mImage_id == mImage_id	&&
			    caches[i]->mVol == mVol) {
				mCache = caches[i]->mCache;
				mDelete = true;
			}
		}

		if (!mCache) {
			mCache = new OmThreadedCachingTile (viewtype, voltype, image_id, vol, shareContext);
			caches.push_back (this);
		}
	}

	static void Refresh () {
		cout << "refershing...." << endl;
		delete new OmCachingThreadedCachingTile ((ViewType)0, (ObjectType)0, (OmId)0, NULL, NULL);
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

