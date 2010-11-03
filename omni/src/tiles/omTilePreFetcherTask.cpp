#include "common/om.hpp"
#include "tiles/omTilePreFetcherTask.hpp"
#include "view2d/omOnScreenTileCoords.h"
#include "view2d/omView2dState.hpp"
#include "tiles/cache/omTileCache.h"

#include <boost/make_shared.hpp>

void OmTilePreFetcherTask::run()
{
	onScreenTileCoords_ = boost::make_shared<OmOnScreenTileCoords>(state_);

	const int maxPrefetchDepth = 32;

	for(int i = 1; i < maxPrefetchDepth; ++i){
		if(shouldExitEarly()){
			return;
		}
		preLoadDepth(i);

		if(shouldExitEarly()){
			return;
		}
		preLoadDepth(-i);
	}
}

void OmTilePreFetcherTask::preLoadDepth(const int depthOffset)
{
	OmTileCoordsAndLocationsPtr tilesCoordsToFetch =
		onScreenTileCoords_->ComputeCoordsAndLocations(depthOffset);

	int count = 0;
	FOR_EACH(tileCL, *tilesCoordsToFetch){

		if(shouldExitEarly()){
			debug(tiles, "OmTilePreFetcherTask: fetched %d tiles, aborting\n", count);
			return;
		}

		++count;

		//debugs(tilesVerbose) << "prefetching: " << tileCL->tileCoord << "\n";

		state_->getCache()->Prefetch(tileCL->tileCoord);
		//prefetcher_->AddTileFetchTask(state, queue);
	}

	debug(tiles, "OmTilePreFetcherTask: fetched %d tiles\n", count);
}


bool OmTilePreFetcherTask::shouldExitEarly(){
	return state_->getCache()->AreDrawersActive();
}
