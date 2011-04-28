#ifndef OM_TILE_PRE_FETCHER_HPP
#define OM_TILE_PRE_FETCHER_HPP

#include "threads/omTaskManager.hpp"

#include <list>

class OmTileDrawer;

class OmTilePreFetcher{
public:
    OmTilePreFetcher();
    ~OmTilePreFetcher();

    void RunTasks(const std::list<OmTileDrawer*>&);
    void ClearTasks();

    void Shutdown();

private:
    OmThreadPool mThreadPool;
};

#endif
