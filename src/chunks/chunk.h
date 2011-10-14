#pragma once

#include "chunks/chunkMipping.hpp"
#include "volume/volumeTypes.h"
#include "chunks/rawChunkSlicer.h"

namespace om {
namespace volume {
    class volume;
}

namespace chunks {

template <typename T>
class chunk {
public:
    chunk(volume::volume* vol, const coords::chunkCoord& coord)
        : vol_(vol)
        , coord_(coord)
        , chunkData_(vol->VolData()->GetChunkPtr())
        , chunkMipping(vol, coord)
        , rawChunkSlicer(128, chunkData_)
    {}

    virtual ~chunk();

    bool ContainsVoxel(const coords::dataCoord& vox) const {
        return vox.volume() == &vol_->CoordinateSystem() &&
               vox.level() == GetLevel() &&
               mipping_.GetExtent().contains(vox);
    }

public:
    const coords::chunkCoord& GetCoordinate() const {
        return coord_;
    }
    int GetLevel() const {
        return coord_.Level;
    }
    const coords::dataCoord GetDimensions() const {
        return mipping_.GetExtent().getUnitDimensions();
    }

    chunkMipping& Mipping(){
        return mipping_;
    }

    inline const T* Data() const {
        return data_;
    }

    inline boost::shared_ptr<tile<T>> GetTile(common::viewType vt, int depth) {
        return slicer_.GetCopyOfTile(vt, depth);
    }

protected:
    const coords::chunkCoord coord_;
    const volume::volume * const vol_;
    const rawChunkSlicer slicer_;

    T* chunkData_;
    chunkMipping mipping_;
};

} // namespace chunks
} // namespace om
