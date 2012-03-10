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

void OmTilePreFetcher::RunTasks(const std::list<OmTileDrawer*>& drawers)
{
    FOR_EACH(iter, drawers)
    {
        OmTileDrawer* drawer = *iter;

        om::shared_ptr<OmTilePreFetcherTask> task =
            om::make_shared<OmTilePreFetcherTask>(drawer->GetState());

        mThreadPool.push_front(task);
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
