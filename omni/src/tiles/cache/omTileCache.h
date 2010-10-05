#ifndef OM_TILE_CACHE_H
#define OM_TILE_CACHE_H

#include "common/om.hpp"
#include "common/omCommon.h"
#include "utility/omLockedPODs.hpp"

class OmTileCoord;
class OmTileCacheChannel;
class OmTileCacheNonChannel;
class OmTileDrawer;
class OmTilePreFetcher;

/**
 *assumes called in a serialized fashion (as main QT GUI thread ensures)
 **/

class OmTileCache {
public:
	void RegisterDrawer(OmTileDrawer*);
	void UnRegisterDrawer(OmTileDrawer*);
	void SetDrawerDone(OmTileDrawer*);
	void WidgetVisibilityChanged(boost::shared_ptr<OmTileDrawer> drawer,
				     const bool visible);

	void Get(OmTileDrawer* drawer,
		 OmTilePtr& tile,
		 const OmTileCoord& key,
		 const om::BlockingRead blocking);

	void RemoveSpaceCoord(const SpaceCoord & coord);

	bool AreDrawersActive(){
		return numDrawersActive_.get() > 0;
	}

private:
	OmTileCache();

	LockedInt32 numDrawersActive_;
	std::map<OmTileDrawer*, bool> drawersActive_;

	boost::shared_ptr<OmTileCacheChannel> cacheChannel_;
	boost::shared_ptr<OmTileCacheNonChannel> cacheNonChannel_;
	boost::shared_ptr<OmTilePreFetcher> preFetcher_;

	bool isChannel(const OmTileCoord& key);
	void setDrawerActive(OmTileDrawer* v);
	void runIdleThreadTask();
	void stopIdleThreadTask();

	void doGet(OmTilePtr& tile,
		   const OmTileCoord& key,
		   const om::BlockingRead blocking);

	friend class OmProjectData; // location where OmTileCache is constructed
	friend class OmTilePreFetcherTask; // access doGet(...)
	friend class OmTileDumper; // access doGet(...)
};

#endif
