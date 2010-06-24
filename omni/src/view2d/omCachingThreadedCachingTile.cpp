#include "view2d/omCachingThreadedCachingTile.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"

OmCachingThreadedCachingTile::OmCachingThreadedCachingTile( ViewType viewtype, OmChannel * chan, OmViewGroupState * vgs ){
	init( viewtype, CHANNEL, chan->GetId(), chan, NULL, vgs );
}

OmCachingThreadedCachingTile::OmCachingThreadedCachingTile( ViewType viewtype, OmSegmentation * seg, OmViewGroupState * vgs ){
	init( viewtype, SEGMENTATION, seg->GetId(), seg, NULL, vgs );
}

void OmCachingThreadedCachingTile::init(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume *vol, const QGLContext *shareContext,
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
