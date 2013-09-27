#pragma once

#include "utility/omSharedPtr.hpp"
#include "zi/omThreads.h"

#include <boost/scoped_ptr.hpp>

class OmView2dState;
class OmOnScreenTileCoords;

class OmTilePreFetcherTask : public zi::runnable {
public:
    OmTilePreFetcherTask(OmView2dState* state);

    void run();

private:
    std::unique_ptr<OmView2dState> state_;
    std::unique_ptr<OmOnScreenTileCoords> onScreenTileCoords_;

    void preLoadDepth(const int depth);

    bool shouldExitEarly();
};

