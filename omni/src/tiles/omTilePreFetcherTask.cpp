#include "common/om.hpp"
#include "view2d/omOnScreenTileCoords.h"
#include "tiles/omTilePreFetcherTask.hpp"
#include "view2d/omView2dState.hpp"

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
