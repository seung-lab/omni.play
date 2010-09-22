#include "tiles/omTilePreFetcher.hpp"
#include "tiles/omTilePreFetcherTask.hpp"
#include "view2d/omView2dState.hpp"
#include "tiles/omTileDrawer.hpp"

#include <boost/make_shared.hpp>

static const int MAX_THREADS = 2;

OmTilePreFetcher::OmTilePreFetcher()
{
	mThreadPool.start(MAX_THREADS);
}

// make a shallow copy of state information to avoid any locking issues
//  with mTotalViewport
boost::shared_ptr<OmView2dState> OmTilePreFetcher::cloneState(OmTileDrawer* d)
{
	return boost::make_shared<OmView2dState>(*(d->getState()));
}

void OmTilePreFetcher::RunTasks(const std::list<OmTileDrawer*>& drawers)
{
	FOR_EACH(iter, drawers){
		boost::shared_ptr<OmTilePreFetcherTask> task =
			boost::make_shared<OmTilePreFetcherTask>(cloneState(*iter));
		mThreadPool.addTaskFront(task);
	}
}

void OmTilePreFetcher::StopTasks()
{
	mThreadPool.clear();
}
