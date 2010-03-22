
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

	mFetchThreadContext = OmStateManager::GetSharedView2dContext(shareContext);
	mShareContext = shareContext;
	mVolType = voltype;
	mImageId = image_id;

        SetCacheName("OmThreadedCachingTile");
}

OmThreadedCachingTile::~OmThreadedCachingTile()
{
	std::cerr << __FUNCTION__ << endl;
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
	// Short curcuit because already low as mip level goes.
	if (rootLevel == tileCoord.Level) {	
		retCoord = tileCoord;
		GetTextureID(p_value, tileCoord);
	}

	OmTileCoord mipTileCoord = tileCoord;

	while (rootLevel >= mipTileCoord.Level) {
		//debug("FIXME", << "Looking for tile for" << mipTileCoord.Level << endl;

		// Try directly below current level.
		mipTileCoord.Level += 1;

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
	TextureIDThreadedCache::Remove(tileCoord);
}

OmTextureID *OmThreadedCachingTile::HandleCacheMiss(const OmTileCoord & key)
{
	//return mesh to cache
	return BindToTextureID(key, this);
}

void OmThreadedCachingTile::SetContinuousUpdate(bool setUpdate)
{
	SetFetchUpdateClearsFetchStack(setUpdate);
}

bool OmThreadedCachingTile::InitializeFetchThread()
{
	return true;
}

void OmThreadedCachingTile::HandleFetchUpdate()
{
	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
}

void OmThreadedCachingTile::subImageTex(shared_ptr < OmTextureID > &texID, int dim, set < DataCoord > &vox,
					QColor & color, int tl)
{
	ReplaceTextureRegion(texID, dim, vox, color, tl);
}
