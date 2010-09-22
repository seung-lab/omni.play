#include "common/om.hpp"
#include "view2d/omOnScreenTileCoords.h"
#include "tiles/omTilePreFetcherTask.hpp"
#include "view2d/omView2dState.hpp"
#include "tiles/cache/omTileCache.h"

void OmTilePreFetcherTask::run()
{
	const int rangeStart = -5;
	const int rangeEnd = 5;

	onScreenTileCoords_ = boost::make_shared<OmOnScreenTileCoords>(state_);

	for(int i = rangeStart; i <= rangeEnd; ++i){
		if(shouldExitEarly()){
			return;
		}

		preLoadDepth(i);
	}
}

void OmTilePreFetcherTask::preLoadDepth(const int depth)
{
	OmTileCoordsAndLocationsPtr tilesCoordsToFetch =
		onScreenTileCoords_->ComputeCoordsAndLocations(depth);

	int count = 0;
	FOR_EACH(tileCL, *tilesCoordsToFetch){
		if(shouldExitEarly()){
			printf("OmTilePreFetcherTask: fetched %d tiles, aborting\n", count);
			return;
		}

		++count;

		//std::cout << "prefetching: " << tileCL->tileCoord << "\n";

		OmTilePtr tile;
		state_->getCache()->doGet(tile, tileCL->tileCoord,
					  OM::BLOCKING);
	}

	printf("OmTilePreFetcherTask: fetched %d tiles\n", count);
}

bool OmTilePreFetcherTask::areDrawersActive()
{
	return state_->getCache()->AreDrawersActive();
}

bool OmTilePreFetcherTask::shouldExitEarly()
{
	return areDrawersActive() || OmCacheManager::AmClosingDown();
}
