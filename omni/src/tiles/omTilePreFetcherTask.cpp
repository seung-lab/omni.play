#include "common/om.hpp"
#include "view2d/omOnScreenTileCoords.h"
#include "tiles/omTilePreFetcherTask.hpp"
#include "view2d/omView2dState.hpp"

#include <boost/array.hpp>

void OmTilePreFetcherTask::run()
{
	onScreenTileCoords_ = boost::make_shared<OmOnScreenTileCoords>(state_);

	static const boost::array<int,10> relativeDepths = { { 1, -1,
							       2, -2,
							       3, -3,
							       4, -4,
							       5, -5 } };

	FOR_EACH(iter, relativeDepths){
		if(shouldExitEarly()){
			return;
		}

		preLoadDepth(*iter);
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
