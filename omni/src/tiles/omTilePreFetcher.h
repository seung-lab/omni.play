#ifndef OM_TILE_PRE_FETCHER_HPP
#define OM_TILE_PRE_FETCHER_HPP

#include "threads/omTaskManager.hpp"
#include <list>

class OmTileDrawer;
class OmView2dState;

class OmTilePreFetcher{
public:
	OmTilePreFetcher();
	~OmTilePreFetcher();

	void RunTasks(const std::list<OmTileDrawer*>&);
	void ClearTasks();

	void Shutdown();

private:
	OmThreadPool mThreadPool;
	om::shared_ptr<OmView2dState> cloneState(OmTileDrawer*);
};

#endif
