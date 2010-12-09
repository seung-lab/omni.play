#include "tiles/omTilePreFetcher.hpp"
#include "tiles/omTilePreFetcherTask.hpp"
#include "view2d/omTileDrawer.h"
#include "zi/omUtility.h"
#include "view2d/omView2dState.hpp"
#include "utility/omSystemInformation.h"

#include <boost/make_shared.hpp>

OmTilePreFetcher::OmTilePreFetcher()
{
	int maxThreads = 2;
	if(OmSystemInformation::get_num_cores() < 3){
		maxThreads = 1;
	}

	mThreadPool.start(maxThreads);
}

// make a shallow copy of state information to avoid any locking issues
//  with mTotalViewport
boost::shared_ptr<OmView2dState> OmTilePreFetcher::cloneState(OmTileDrawer* d)
{
	return boost::make_shared<OmView2dState>(*(d->GetState()));
}

void OmTilePreFetcher::RunTasks(const std::list<OmTileDrawer*>& drawers)
{
	//init the map
	FOR_EACH(iter, drawers){
		boost::shared_ptr<OmTilePreFetcherTask> task =
			boost::make_shared<OmTilePreFetcherTask>(cloneState(*iter));
		mThreadPool.addTaskFront(task);
	}
}

void OmTilePreFetcher::StopTasks()
{
	mThreadPool.clear();
	//kill map
}

#if 0
void OmTilePreFetcher::AddTileFetchTask(state, coord)
{
	//lock
	//add to map based on depth
	//unlock
}
#endif

