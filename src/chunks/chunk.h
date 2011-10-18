#pragma once

#include "chunks/mipping.hpp"
#include "volume/volumeTypes.h"

namespace om {
namespace volume {
    class volume;
}

namespace chunks {
class mipping;
class dataInterface;

class chunk {
public:
    chunk(volume::volume* vol, const coords::chunkCoord& coord);

public:
    const coords::chunkCoord& GetCoordinate() const {
        return coord_;
    }
    int GetLevel() const {
        return coord_.Level;
    }

    coords::dataCoord GetDimensions() const;

    mipping& Mipping() {
        return *mipping_;
    }

    inline dataInterface* Data() const {
        return chunkData_.get();
    }

protected:
    const coords::chunkCoord coord_;
    const volume::volume* const vol_;
    boost::scoped_ptr<dataInterface> chunkData_;
    boost::scoped_ptr<mipping> mipping_;
};

} // namespace chunks
} // namespace om
