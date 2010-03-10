
#include "omThreadedCachingTile.h"

#include "common/omThreads.h"
#include "system/omSystemTypes.h"
#include "system/omEventManager.h"
#include "system/events/omViewEvent.h"
#include "system/omStateManager.h"

#include "volume/omVolumeTypes.h"

#include <QColor>
#include <QGLContext>
#include "common/omDebug.h"

#define DEBUG 0

OmThreadedCachingTile::OmThreadedCachingTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume * vol,
					     const QGLContext * shareContext)
:OmTile(viewtype, voltype, image_id, vol), TextureIDThreadedCache(VRAM_CACHE_GROUP, true)
{
	// omView2d passes in its own context

	//std::cerr << __FUNCTION__ << endl;

	isSecondMipVolume = false;

	mFetchThreadContext = OmStateManager::GetSharedView2dContext(shareContext);
	mShareContext = shareContext;
	mVolType = voltype;
	mImageId = image_id;

	/** Set The Name of the Cache */
        SetCacheName("OmThreadedCachingTile");

}

OmThreadedCachingTile::~OmThreadedCachingTile()
{
	std::cerr << __FUNCTION__ << endl;
	//if(mFetchThreadContext) delete mFetchThreadContext;
}

void OmThreadedCachingTile::GetTextureID(shared_ptr < OmTextureID > &p_value, const OmTileCoord & tileCoord, bool block)
{

	TextureIDThreadedCache::Get(p_value, tileCoord, block);
	return;
}

// Called at the highest miplevel will force the entire octree into memory so an initial
// calling at a medium mip level might be a good idea...
void OmThreadedCachingTile::GetTextureIDDownMip(shared_ptr < OmTextureID > &p_value, const OmTileCoord & tileCoord,
						int rootLevel, OmTileCoord & retCoord)
{
	// //debug("genone","OmCachingTile::GetTextureID: " << tileCoord.get<0>() << " " << tileCoord.get<1>());
	//std::cerr << __FUNCTION__ << endl;

	if (rootLevel == tileCoord.Level) {	// Short curcuit because already low as mip level goes.
		retCoord = tileCoord;
		GetTextureID(p_value, tileCoord);
	}

	OmTileCoord mipTileCoord = tileCoord;

	while (rootLevel >= mipTileCoord.Level) {
		//debug("FIXME", << "Looking for tile for" << mipTileCoord.Level << endl;
		// Try directly below current level.
		mipTileCoord.Level += 1;
		//shared_ptr<OmTextureID> id;
		TextureIDThreadedCache::Get(p_value, mipTileCoord);
		if (p_value) {
			//debug("FIXME", << "tile " << mipTileCoord.Level << " found" << endl;
			retCoord = mipTileCoord;
			return;
		}
	}

	retCoord = tileCoord;
	TextureIDThreadedCache::Get(p_value, tileCoord);
	return;
}

void OmThreadedCachingTile::StoreTextureID(const OmTileCoord & tileCoord, OmTextureID * texID)
{
	std::cerr << __FUNCTION__ << endl;
	TextureIDThreadedCache::Add(tileCoord, texID);
}

void OmThreadedCachingTile::Remove(const OmTileCoord & tileCoord)
{
	//std::cerr << __FUNCTION__ << endl;
	TextureIDThreadedCache::Remove(tileCoord);
}

OmTextureID *OmThreadedCachingTile::HandleCacheMiss(const OmTileCoord & key)
{
	//std::cerr << __FUNCTION__ << endl;
	//debug("genone","OmThreadedCachingTile::HandleCacheMiss: %i\n ",key.Level);

//      //debug("genone","OmThreadedCachingTile::HandleCacheMiss: set context");
//      OmStateManager::MakeContextCurrent(mFetchThreadContext);

	//return mesh to cache
	debug("crazycash","I'm fetching a Texture ID Boss!\n");  
	return BindToTextureID(key, this);
	// OmTile: OmTextureID* BindToTextureID(const OmTileCoord &key);
}

void OmThreadedCachingTile::SetContinuousUpdate(bool setUpdate)
{
	//std::cerr << __FUNCTION__ << endl;
	SetFetchUpdateClearsFetchStack(setUpdate);
}

bool OmThreadedCachingTile::InitializeFetchThread()
{
	//std::cerr << __FUNCTION__ << endl;
	//debug("genone","OmThreadedCachingTile::InitializeFetchThread: set context\n");

	//aquire and set shared context
	//mFetchThreadContext = OmStateManager::GetSharedView2dContext(mShareContext);
	//OmStateManager::MakeContextCurrent(mFetchThreadContext);

	return true;
}

void OmThreadedCachingTile::HandleFetchUpdate()
{
	//std::cerr << "STARTING " << __FUNCTION__ << endl;

	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));

	//std::cerr << "ENDING " << __FUNCTION__ << endl;

	//debug("genone","OmMipMeshManager::FetchUpdate: redraw sent\n");
}

//void OmThreadedCachingTile::ClearCache() { 
//      TextureIDThreadedCache::Clean(true); 
//}

void OmThreadedCachingTile::setSecondMipVolume(ObjectType secondtype, OmId second_id, OmMipVolume * secondvol)
{
	//std::cerr << __FUNCTION__ << endl;
	isSecondMipVolume = true;
	////debug("genone","secondvol is enabled = " << secondvol->GetCacheMaxSize());
	AddOverlay(secondtype, second_id, secondvol);
}

void OmThreadedCachingTile::subImageTex(shared_ptr < OmTextureID > &texID, int dim, set < DataCoord > &vox,
					QColor & color, int tl)
{
	//std::cerr << __FUNCTION__ << endl;
	ReplaceTextureRegion(texID, dim, vox, color, tl);
}

void OmThreadedCachingTile::SetMaxCacheSize(int bytes)
{
	//std::cerr << __FUNCTION__ << endl;
	//TextureIDThreadedCache::SetMaxSize(bytes);
}
