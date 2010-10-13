#ifndef OM_TILE_PRE_FETCHER_HPP
#define OM_TILE_PRE_FETCHER_HPP

#include "utility/omThreadPool.hpp"
#include <list>

class OmTileDrawer;
class OmView2dState;

class OmTilePreFetcher{
public:
	OmTilePreFetcher();

	void RunTasks(const std::list<OmTileDrawer*>&);
	void StopTasks();

private:
	OmThreadPool mThreadPool;
	boost::shared_ptr<OmView2dState> cloneState(OmTileDrawer*);
};

#endif
