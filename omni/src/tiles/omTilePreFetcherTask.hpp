#ifndef OM_TILE_PRE_FETCHER_TASK_HPP
#define OM_TILE_PRE_FETCHER_TASK_HPP

#include "tiles/cache/omTileCache.h"
#include "view2d/omView2dState.hpp"

#include <zi/threads>

class OmView2dState;
class OmOnScreenTileCoords;

class OmTilePreFetcherTask : public zi::Runnable {
public:
	OmTilePreFetcherTask(const boost::shared_ptr<OmView2dState>& state)
		: state_(state) {}

	void run();

private:
	const boost::shared_ptr<OmView2dState> state_;
	boost::shared_ptr<OmOnScreenTileCoords> onScreenTileCoords_;

	void preLoadDepth(const int depth);

	bool areDrawersActive(){
		return state_->getCache()->AreDrawersActive();
	}

	bool shouldExitEarly(){
		return areDrawersActive() || OmCacheManager::AmClosingDown();
	}
};

#endif
