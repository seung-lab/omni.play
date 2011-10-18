#pragma once

#include "common/std.h"

namespace om {
namespace volume {
class volume;
}

namespace om {
namespace tiles {

class tile {
public:
    tile(const volume::volume* vol,
         coords::chunkCoord coord,
         common::viewType view,
         int depth)
        : vol_(vol)
        , coord_(coord)
        , view_(view)
        , depth_(depth)
    {}

    void loadData();

    const uint32_t* data() const {
        return data_.get();
    }

    inline const volume::volume* volume() const {
        return vol_;
    }

    inline const coords::chunkCoord coord() const {
        return coord_;
    }

    inline common::viewType view() const {
        return view_;
    }

    inline int depth() const {
        return depth_;
    }
private:
    const volume::volume* vol_;
    const coords::chunkCoord coord_;
    const common::viewType view_;
    const int depth_;

    boost::scoped_ptr<uint32_t> data_;
};

} // namespace tiles
} // namespace om
