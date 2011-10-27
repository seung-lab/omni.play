#pragma once

#include "common/std.h"

namespace om {
namespace volume { class volume; }

namespace tiles {

template <typename T>
class tile {
public:
    tile(volume::volume* vol,
         coords::chunkCoord coord,
         common::viewType view,
         int depth)
        : vol_(vol)
        , coord_(coord)
        , view_(view)
        , depth_(depth)
    {}

    void loadData();

    T* data() {
        return data_.get();
    }

    const T* data() const {
        return data.get();
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
    volume::volume* vol_;
    const coords::chunkCoord coord_;
    const common::viewType view_;
    const int depth_;

    boost::shared_ptr<T> data_;
};

} // namespace tiles
} // namespace om
