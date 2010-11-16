#ifndef OM_TILE_CACHE_H
#define OM_TILE_CACHE_H

#include "common/om.hpp"
#include "common/omCommon.h"
#include "utility/omLockedPODs.hpp"

#include <QApplication>

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
		 const om::Blocking blocking);

	void Prefetch(const OmTileCoord& key);

	void RemoveSpaceCoord(const SpaceCoord & coord);

	bool AreDrawersActive();

	void Clear();

private:
	OmTileCache();

	boost::shared_ptr<OmTileCacheChannel> cacheChannel_;
	boost::shared_ptr<OmTileCacheNonChannel> cacheNonChannel_;
	boost::shared_ptr<OmTilePreFetcher> preFetcher_;

	std::map<OmTileDrawer*, bool> drawersActive_;
	LockedInt32 numDrawersActive_;

	bool isChannel(const OmTileCoord& key);
	void setDrawerActive(OmTileDrawer* v);
	void runIdleThreadTask();
	void stopIdleThreadTask();

	void doGet(OmTilePtr& tile,
			   const OmTileCoord& key,
			   const om::Blocking blocking);

	friend class OmProjectData; // location where OmTileCache is constructed
	friend class OmTileDumper; // access doGet(...)
};

#endif
