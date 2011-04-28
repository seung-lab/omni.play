#ifndef OM_TILE_PRE_FETCHER_TASK_HPP
#define OM_TILE_PRE_FETCHER_TASK_HPP

#include "utility/omSharedPtr.hpp"
#include "zi/omThreads.h"

class OmView2dState;
class OmOnScreenTileCoords;

class OmTilePreFetcherTask : public zi::runnable {
public:
    OmTilePreFetcherTask(const om::shared_ptr<OmView2dState>& state)
        : state_(state) {}

    void run();

private:
    const om::shared_ptr<OmView2dState> state_;
    om::shared_ptr<OmOnScreenTileCoords> onScreenTileCoords_;

    void preLoadDepth(const int depth);

    bool shouldExitEarly();
};

#endif
