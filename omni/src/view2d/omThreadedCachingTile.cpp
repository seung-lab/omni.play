#include "system/cache/omTileCache.h"
#include "common/omDebug.h"
#include "system/events/omViewEvent.h"
#include "system/omEventManager.h"
#include "system/omStateManager.h"
#include "view2d/omThreadedCachingTile.h"
#include "volume/omVolume.h"

#include <QColor>
#include <QGLContext>

OmThreadedCachingTile::OmThreadedCachingTile(ViewType viewtype, 
					     ObjectType voltype, 
					     OmId image_id, 
					     OmMipVolume * vol,
                                             const QGLContext * shareContext,
					     OmViewGroupState * vgs)
	: OmTile(viewtype, voltype, image_id, vol, vgs)
	, mDataCache(new OmTileCache(this))
{
	// omView2d passes in its own context

	mShareContext = shareContext;
	mVolType = voltype;
	mImageId = image_id;
}

OmThreadedCachingTile::~OmThreadedCachingTile()
{
	delete mDataCache;
}

void OmThreadedCachingTile::GetTextureID(OmTextureIDPtr& p_value, 
					 const OmTileCoord & tileCoord, 
					 bool block)
{
	mDataCache->Get(p_value, tileCoord, block);
}

// Called at the highest miplevel will force the entire octree into memory so an initial
// calling at a medium mip level might be a good idea...
void OmThreadedCachingTile::GetTextureIDDownMip(OmTextureIDPtr& p_value, 
						const OmTileCoord & tileCoord,
						int rootLevel, 
						OmTileCoord & retCoord)
{
	// Short circuit because already low as mip level goes.
	if (rootLevel == tileCoord.Level) {	
		retCoord = tileCoord;
		GetTextureID(p_value, tileCoord);
	}

	OmTileCoord mipTileCoord = tileCoord;

	while (rootLevel >= mipTileCoord.Level) {

		// Try directly below current level.
		mipTileCoord.Level += 1;

		mDataCache->Get(p_value, mipTileCoord, false);
		if (p_value) {
			//debug("FIXME", << "tile " << mipTileCoord.Level << " found" << endl;
			retCoord = mipTileCoord;
			return;
		}
	}

	retCoord = tileCoord;
	mDataCache->Get(p_value, tileCoord, false);
}
