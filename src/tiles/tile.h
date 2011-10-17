#pragma once

#include "common/std.h"

namespace om {
namespace volume {
class volume;
}

namespace om {
namespace tiles {

template <typename T>
class tile {
public:
    tile(const volume::volume* vol,
         coords::chunkCoord coord,
         common::viewType view,
         int depth,
         T* data)
        : vol_(vol)
        , coord_(coord)
        , view_(view)
        , depth_(depth)
    {}

    ~tile() {
        delete data_;
    }

    const T* data() const {
        return data_;
    }

    inline const volume::volume* volume() const {
        return vol_;
    }

    inline const coords::chunkCoord* coord() const {
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

    T* data_;
};

} // namespace tiles
} // namespace om
