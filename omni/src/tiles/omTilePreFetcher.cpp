#include "tiles/omTilePreFetcher.h"
#include "tiles/omTilePreFetcherTask.hpp"
#include "utility/omSystemInformation.h"
#include "view2d/omTileDrawer.hpp"
#include "view2d/omView2dState.hpp"
#include "zi/omUtility.h"

OmTilePreFetcher::OmTilePreFetcher()
{
    int maxThreads = 2;
    if(OmSystemInformation::get_num_cores() < 3){
        maxThreads = 1;
    }

    mThreadPool.start(maxThreads);
}

OmTilePreFetcher::~OmTilePreFetcher()
{
    Shutdown();
}

// make a shallow copy of state information to avoid any locking issues
//  with mTotalViewport
om::shared_ptr<OmView2dState> OmTilePreFetcher::cloneState(OmTileDrawer* d)
{
    return om::make_shared<OmView2dState>(*(d->GetState()));
}

void OmTilePreFetcher::RunTasks(const std::list<OmTileDrawer*>& drawers)
{
    //init the map
    FOR_EACH(iter, drawers){
        om::shared_ptr<OmTilePreFetcherTask> task =
            om::make_shared<OmTilePreFetcherTask>(cloneState(*iter));
        mThreadPool.addTaskFront(task);
    }
}

void OmTilePreFetcher::ClearTasks()
{
    mThreadPool.clear();
}

void OmTilePreFetcher::Shutdown()
{
    mThreadPool.clear();
    mThreadPool.stop();
}
