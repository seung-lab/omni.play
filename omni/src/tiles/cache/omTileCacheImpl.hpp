#ifndef OM_TILE_CACHE_IMPL_HPP
#define OM_TILE_CACHE_IMPL_HPP

#include "common/om.hpp"
#include "common/omCommon.h"
#include "system/omStateManager.h"
#include "tiles/cache/omTileCacheChannel.hpp"
#include "tiles/cache/omTileCacheSegmentation.hpp"
#include "tiles/omTilePreFetcher.hpp"
#include "utility/omLockedPODs.hpp"
#include "view2d/omTileDrawer.h"
#include "view2d/omView2dState.hpp"

#include <QApplication>

DECLARE_ZiARG_bool(noTilePrefetch);

/**
 * assumes called in a serialized fashion (as main QT GUI thread ensures)
 *
 * prefetcher calls in threaded-manner....
 **/

class OmTileCacheImpl {
public:
	void RegisterDrawer(OmTileDrawer* d){
		drawersActive_[d] = false;
	}

	void UnRegisterDrawer(OmTileDrawer* d)
	{
		if(drawersActive_[d]){
			--numDrawersActive_;
		}
		drawersActive_.erase(d);
		runIdleThreadTask();
	}

	void SetDrawerDone(OmTileDrawer* d)
	{
		if(!drawersActive_[d]){
			return;
		}

		drawersActive_[d] = false;
		--numDrawersActive_;
		runIdleThreadTask();
	}

	void WidgetVisibilityChanged(boost::shared_ptr<OmTileDrawer> drawer,
								 const bool visible)
	{
		if(visible){
			setDrawerActive(drawer.get());
		}else{
			SetDrawerDone(drawer.get());
		}
	}

	void Get(OmTileDrawer* drawer,
			 OmTilePtr& tile,
			 const OmTileCoord& key,
			 const om::Blocking blocking)
	{
		setDrawerActive(drawer);
		doGet(tile, key, blocking);
	}

	void Prefetch(const OmTileCoord& key)
	{
		if(isChannel(key)){
			cacheChannel_->Prefetch(key);
		} else {
			cacheSegmentation_->Prefetch(key);
		}
	}

	void RemoveSpaceCoord(const SpaceCoord & coord){
		cacheSegmentation_->RemoveSpaceCoord(coord);
	}

	bool AreDrawersActive()
	{
		return numDrawersActive_.get() > 0 ||
			OmCacheManager::AmClosingDown() ||
			QApplication::mouseButtons() != Qt::NoButton;
	}

	void Clear()
	{
		cacheChannel_->Clear();
		cacheSegmentation_->Clear();
	}

	void ClearChannel(){
		cacheChannel_->Clear();
	}

private:
	boost::shared_ptr<OmTileCacheChannel> cacheChannel_;
	boost::shared_ptr<OmTileCacheSegmentation> cacheSegmentation_;
	boost::shared_ptr<OmTilePreFetcher> preFetcher_;

	std::map<OmTileDrawer*, bool> drawersActive_;
	LockedInt32 numDrawersActive_;

	OmTileCacheImpl()
		: cacheChannel_(boost::make_shared<OmTileCacheChannel>())
		, cacheSegmentation_(boost::make_shared<OmTileCacheSegmentation>())
		, preFetcher_(boost::make_shared<OmTilePreFetcher>())
	{
		numDrawersActive_.set(0);
	}

	bool isChannel(const OmTileCoord& key){
		return CHANNEL == key.getVolume()->getVolumeType();
	}

	void setDrawerActive(OmTileDrawer* d)
	{
		if(drawersActive_[d]){
			return;
		}

		drawersActive_[d] = true;
		++numDrawersActive_;
		stopIdleThreadTask();
	}

	void runIdleThreadTask()
	{
		if(ZiARG_noTilePrefetch){
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

	void stopIdleThreadTask()
	{
		if(!AreDrawersActive()){
			return;
		}

		preFetcher_->StopTasks();
	}

	void doGet(OmTilePtr& tile,
			   const OmTileCoord& key,
			   const om::Blocking blocking)
	{
		if(isChannel(key)){
			cacheChannel_->Get(tile, key, blocking);
		} else {
			cacheSegmentation_->Get(tile, key, blocking);
		}
	}

	friend class OmTileCache;
};

#endif
