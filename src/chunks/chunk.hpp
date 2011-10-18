#pragma once

#include "chunks/mipping.hpp"
#include "volume/volumeTypes.h"
#include "chunks/rawChunkSlicer.hpp"
#include "tiles/tile.h"

namespace om {
namespace volume {
    class volume;
}

namespace chunks {

class dataInterface;

class chunk {
public:
    chunk(volume::volume* vol, const coords::chunkCoord& coord)
        : vol_(vol)
        , coord_(coord)
        , chunkData_(vol->VolData()->GetChunkPtr())
        , mipping(vol, coord)
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

    mipping& Mipping(){
        return mipping_;
    }

    inline const T* Data() const {
        return data_;
    }

protected:
    const coords::chunkCoord coord_;
    const volume::volume* const vol_;
    boost::scoped_ptr<dataInterface> chunkData_;
    mipping mipping_;
};

} // namespace chunks
} // namespace om
