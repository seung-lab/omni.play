
#include "omThreadedCachingTile.h"

#include "common/omThreads.h"
#include "system/omSystemTypes.h"
#include "system/omEventManager.h"
#include "system/events/omViewEvent.h"
#include "system/omStateManager.h"

#include "volume/omVolumeTypes.h"

#include <QColor>
#include <QGLContext>


#define DEBUG 0

OmThreadedCachingTile::OmThreadedCachingTile(ViewType viewtype, ObjectType voltype, OmId image_id, OmMipVolume *vol, const QGLContext *shareContext)
: OmTile(viewtype, voltype, image_id, vol), TextureIDThreadedCache(VRAM_CACHE_GROUP, true) {
	// omView2d passes in its own context

	//std::cerr << __FUNCTION__ << endl;
	
	isSecondMipVolume = false;
	
	mFetchThreadContext = OmStateManager::GetSharedView2dContext(shareContext);
	mShareContext = shareContext;
	mVolType = voltype;
	mImageId = image_id;
}

OmThreadedCachingTile::~OmThreadedCachingTile() {
	std::cerr << __FUNCTION__ << endl;
	//if(mFetchThreadContext) delete mFetchThreadContext;
}

shared_ptr<OmTextureID> OmThreadedCachingTile::GetTextureID(const OmTileCoord &tileCoord, bool block) {
	// DOUT("OmCachingTile::GetTextureID: " << tileCoord.get<0>() << " " << tileCoord.get<1>());
	//std::cerr << __FUNCTION__ << endl;
	return TextureIDThreadedCache::Get(tileCoord, block);
}

// Called at the highest miplevel will force the entire octree into memory so an initial
// calling at a medium mip level might be a good idea...
shared_ptr<OmTextureID> OmThreadedCachingTile::GetTextureIDDownMip(const OmTileCoord &tileCoord, int rootLevel, OmTileCoord &retCoord) {
	// DOUT("OmCachingTile::GetTextureID: " << tileCoord.get<0>() << " " << tileCoord.get<1>());
	//std::cerr << __FUNCTION__ << endl;

	
	if (rootLevel == tileCoord.Level) {	// Short curcuit because already low as mip level goes.
		retCoord = tileCoord;
		return GetTextureID (tileCoord);
	}

	OmTileCoord mipTileCoord = tileCoord;

	while (rootLevel >= mipTileCoord.Level) {
		//cout << "Looking for tile for" << mipTileCoord.Level << endl;
		// Try directly below current level.
		mipTileCoord.Level += 1;
		shared_ptr<OmTextureID>id = TextureIDThreadedCache::Get(mipTileCoord);
		if (id) {
			//cout << "tile " << mipTileCoord.Level << " found" << endl;
			retCoord = mipTileCoord;
			return id;
		}
	}

	retCoord = tileCoord;
	return TextureIDThreadedCache::Get(tileCoord);
}


void OmThreadedCachingTile::StoreTextureID(const OmTileCoord &tileCoord, OmTextureID* texID) {
	std::cerr << __FUNCTION__ << endl;
	TextureIDThreadedCache::Add(tileCoord, texID);
}

void OmThreadedCachingTile::Remove(const OmTileCoord &tileCoord) {
	//std::cerr << __FUNCTION__ << endl;
	TextureIDThreadedCache::Remove(tileCoord);
}

OmTextureID* OmThreadedCachingTile::HandleCacheMiss(const OmTileCoord &key) {
	//std::cerr << __FUNCTION__ << endl;
	DOUT("OmThreadedCachingTile::HandleCacheMiss: " << key.Level << " " << key.Coordinate);
	
//	DOUT("OmThreadedCachingTile::HandleCacheMiss: set context");
//	OmStateManager::MakeContextCurrent(mFetchThreadContext);
	
	//return mesh to cache
	DOUT("OmThreadedCachingTile::HandleCacheMiss: returning texture");	
	return BindToTextureID(key);
	// OmTile: OmTextureID* BindToTextureID(const OmTileCoord &key);
}

void OmThreadedCachingTile::SetContinuousUpdate(bool setUpdate) {
	//std::cerr << __FUNCTION__ << endl;
	SetFetchUpdateClearsFetchStack(setUpdate);
}

bool
OmThreadedCachingTile::InitializeFetchThread() {
	//std::cerr << __FUNCTION__ << endl;
	DOUT("OmThreadedCachingTile::InitializeFetchThread: set context");
	
	//aquire and set shared context
	//mFetchThreadContext = OmStateManager::GetSharedView2dContext(mShareContext);
	//OmStateManager::MakeContextCurrent(mFetchThreadContext);
	
	return true;
}

void 
OmThreadedCachingTile::HandleFetchUpdate() {
	//std::cerr << "STARTING " << __FUNCTION__ << endl;
	
	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));	
	
	//std::cerr << "ENDING " << __FUNCTION__ << endl;

	DOUT("OmMipMeshManager::FetchUpdate: redraw sent");
}


//void OmThreadedCachingTile::ClearCache() { 
//	TextureIDThreadedCache::Clean(true); 
//}


void OmThreadedCachingTile::setSecondMipVolume(ObjectType secondtype, OmId second_id, OmMipVolume *secondvol) {
	//std::cerr << __FUNCTION__ << endl;
	isSecondMipVolume = true;
	//DOUT("secondvol is enabled = " << secondvol->GetCacheMaxSize());
	AddOverlay(secondtype, second_id, secondvol);
}

void OmThreadedCachingTile::subImageTex(shared_ptr<OmTextureID> &texID, int dim, set< DataCoord > &vox, QColor &color, int tl) {
	//std::cerr << __FUNCTION__ << endl;
	ReplaceTextureRegion(texID, dim, vox, color, tl);
}

void OmThreadedCachingTile::subFullImageTex(shared_ptr<OmTextureID> &texID, DataCoord firstCoord, int tl) {
	//std::cerr << __FUNCTION__ << endl;
	ReplaceFullTextureRegion(texID, firstCoord, tl);
}

void OmThreadedCachingTile::SetMaxCacheSize(int bytes) {
	//std::cerr << __FUNCTION__ << endl;
	//TextureIDThreadedCache::SetMaxSize(bytes);
}
