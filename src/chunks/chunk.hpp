#pragma once

#include "volume/volume.h"
#include "volume/io/volumeData.h"
#include "chunks/mipping.hpp"
#include "volume/volumeTypes.h"
#include "chunks/rawChunkSlicer.hpp"
#include "tiles/tile.h"

namespace om {
namespace chunks {

template <typename T>
class chunk {
public:
    chunk(volume::volume<T>* vol, const coords::chunkCoord& coord)
        : vol_(vol)
        , coord_(coord)
        , chunkData_(vol->VolData()->GetChunkPtr())
        , mipping_(vol, coord)
        , slicer_(128, chunkData_)
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

    inline mipping& Mipping(){
        return mipping_;
    }

    inline const T* Data() const {
        return data_;
    }

    inline boost::shared_ptr<tiles::tile<T> > GetTile(common::viewType vt, int depth) {
        T* tileData = slicer_.GetCopyOfTile(vt, depth);
        boost::shared_ptr<tiles::tile<T>> ret =
            boost::make_shared<tiles::tile<T> >(vol_, coord_, vt, depth, tileData);
        return ret;
    }

protected:
    const coords::chunkCoord coord_;
    const volume::volume<T> * const vol_;
    const rawChunkSlicer<T> slicer_;

    T* chunkData_;
    mipping mipping_;
};

} // namespace chunks
} // namespace om
