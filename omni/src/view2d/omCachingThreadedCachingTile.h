#ifndef OM_CACHING_THREADED_CACHING_TILE_H
#define OM_CACHING_THREADED_CACHING_TILE_H

#include "view2d/omThreadedCachingTile.h"

class OmCachingThreadedCachingTile
{
public:
	OmCachingThreadedCachingTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume *vol, const QGLContext *shareContext,
				     OmViewGroupState * vgs)
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
			mCache = new OmThreadedCachingTile (viewtype, voltype, image_id, vol, shareContext, vgs );
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
