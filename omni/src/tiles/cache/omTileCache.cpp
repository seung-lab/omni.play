#include "view2d/omView2dState.hpp"
#include "tiles/omTilePreFetcher.hpp"
#include "tiles/omTileDrawer.hpp"
#include "tiles/cache/omTileCache.h"
#include "tiles/cache/omTileCacheImpl.h"
#include "system/omStateManager.h"

#include <boost/make_shared.hpp>

OmTileCache::OmTileCache()
	: cacheChannel_(boost::make_shared<OmTileCacheChannel>())
	, cacheNonChannel_(boost::make_shared<OmTileCacheNonChannel>())
	, preFetcher_(boost::make_shared<OmTilePreFetcher>())
{
	numDrawersActive_.set(0);
}

void OmTileCache::Get(OmTileDrawer* drawer,
					  OmTilePtr& tile,
					  const OmTileCoord& key,
					  const om::BlockingRead blocking)
{
	setDrawerActive(drawer);
	doGet(tile, key, blocking);
}

void OmTileCache::doGet(OmTilePtr& tile,
						const OmTileCoord& key,
						const om::BlockingRead blocking)
{
	if(isChannel(key)){
		cacheChannel_->Get(tile, key, blocking);
	} else {
		cacheNonChannel_->Get(tile, key, blocking);
	}
}

void OmTileCache::RemoveSpaceCoord(const SpaceCoord & coord){
	cacheNonChannel_->RemoveSpaceCoord(coord);
}

bool OmTileCache::isChannel(const OmTileCoord& key){
	return CHANNEL == key.getVolume()->getVolumeType();
}

void OmTileCache::RegisterDrawer(OmTileDrawer* d){
	drawersActive_[d] = false;
}

void OmTileCache::UnRegisterDrawer(OmTileDrawer* d)
{
	if(drawersActive_[d]){
		--numDrawersActive_;
	}
	drawersActive_.erase(d);
	runIdleThreadTask();
}

void OmTileCache::setDrawerActive(OmTileDrawer* d)
{
	if(drawersActive_[d]){
		return;
	}

	drawersActive_[d] = true;
	++numDrawersActive_;
	stopIdleThreadTask();
}

void OmTileCache::SetDrawerDone(OmTileDrawer* d)
{
	if(!drawersActive_[d]){
		return;
	}

	drawersActive_[d] = false;
	--numDrawersActive_;
	runIdleThreadTask();
}

void OmTileCache::runIdleThreadTask()
{
	if(OmStateManager::getNoTilePrefetch()){
		return;
	}

	if(AreDrawersActive()){
		return;
	}

	std::list<OmTileDrawer*> drawers;
	FOR_EACH(iter, drawersActive_){
		drawers.push_back(iter->first);
	}
	preFetcher_->RunTasks(drawers);
}

void OmTileCache::stopIdleThreadTask()
{
	if(!AreDrawersActive()){
		return;
	}

	preFetcher_->StopTasks();
}

void OmTileCache::WidgetVisibilityChanged(boost::shared_ptr<OmTileDrawer> drawer,
										  const bool visible)
{
	if(visible){
		setDrawerActive(drawer.get());
	}else{
		SetDrawerDone(drawer.get());
	}
}

bool OmTileCache::AreDrawersActive()
{
	return numDrawersActive_.get() > 0 ||
		OmCacheManager::AmClosingDown() ||
		QApplication::mouseButtons() != Qt::NoButton;
}
