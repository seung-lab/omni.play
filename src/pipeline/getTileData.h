#pragma once

#include "common/common.h"
#include "pipeline/stage.hpp"
#include "boost/scoped_ptr.hpp"

namespace om {
namespace volume { class channel; }
namespace tiles { class tile; }
namespace pipeline {

class getTileData : public out_stage<char>
{
private:
    int outSize_;
    volume::channel* chan_;
    coords::chunkCoord coord_;
    common::viewType view_;
    int depth_;
    char* data_;

public:
    getTileData(volume::channel* chan,
                coords::chunkCoord coord,
                common::viewType view,
                int depth)
        : chan_(chan)
        , coord_(coord)
        , view_(view)
        , depth_(depth)
    {}

    char* operator()();

    inline int out_size() {
        return outSize_;
    }

    void cleanup();
};

}
}
