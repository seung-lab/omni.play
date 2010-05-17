#include "volume/omVolume.h"
#include "omThreadedCachingTile.h"

#include "system/omEventManager.h"
#include "system/events/omViewEvent.h"
#include "system/omStateManager.h"

#include <QColor>
#include <QGLContext>
#include "common/omDebug.h"

OmThreadedCachingTile::OmThreadedCachingTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume * vol,
                                             const QGLContext * shareContext)
  : OmTile(viewtype, voltype, image_id, vol), 
    TextureIDThreadedCache(VRAM_CACHE_GROUP, true)
{
	// omView2d passes in its own context

	mShareContext = shareContext;
	mVolType = voltype;
	mImageId = image_id;

        SetCacheName("OmThreadedCachingTile");
        int chunkDim = vol->GetChunkDimension();
        SetObjectSize(chunkDim*chunkDim*4);
}

OmThreadedCachingTile::~OmThreadedCachingTile()
{
}

void OmThreadedCachingTile::GetTextureID(QExplicitlySharedDataPointer < OmTextureID > &p_value, const OmTileCoord & tileCoord, bool block)
{
	TextureIDThreadedCache::Get(p_value, tileCoord, block);
}

// Called at the highest miplevel will force the entire octree into memory so an initial
// calling at a medium mip level might be a good idea...
void OmThreadedCachingTile::GetTextureIDDownMip(QExplicitlySharedDataPointer < OmTextureID > &p_value, const OmTileCoord & tileCoord,
						int rootLevel, OmTileCoord & retCoord)
{
	// Short circuit because already low as mip level goes.
	if (rootLevel == tileCoord.Level) {	
		retCoord = tileCoord;
		GetTextureID(p_value, tileCoord);
	}

	OmTileCoord mipTileCoord = tileCoord;

	while (rootLevel >= mipTileCoord.Level) {
		//debug("FIXME", << "Looking for tile for" << mipTileCoord.Level << endl;

		// Try directly below current level.
		mipTileCoord.Level += 1;

		TextureIDThreadedCache::Get(p_value, mipTileCoord, false);
		if (p_value) {
			//debug("FIXME", << "tile " << mipTileCoord.Level << " found" << endl;
			retCoord = mipTileCoord;
			return;
		}
	}

	retCoord = tileCoord;
	TextureIDThreadedCache::Get(p_value, tileCoord, false);
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

void OmThreadedCachingTile::subImageTex( set < DataCoord > &vox )
{
	ReplaceTextureRegion(vox);
}
