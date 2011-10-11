#pragma once

/**
 *  chunk encapsulates a 3D-matrix of image data (typically 128^3) and
 *    its associated size/positioning metadata.
 *
 *  Brett Warne - bwarne@mit.edu - 2/24/09
 *  Michael Purcaro - purcaro@gmail.com - 1/29/11
 */

#include "chunks/chunkMipping.hpp"
#include "volume/volumeTypes.h"

namespace om { 
    
namespace volume {
    class channel;
    class channelImpl;
    class segmentation;
}
    
namespace chunks { 
    
class dataInterface;

class chunk {
public:
    chunk(volume::channel* vol, const coords::chunkCoord& coord);
    chunk(volume::channelImpl* vol, const coords::chunkCoord& coord);
    chunk(volume::segmentation* vol, const coords::chunkCoord& coord);

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

    om::chunks::dataInterface* Data(){
        return chunkData_.get();
    }

    chunkMipping& Mipping(){
        return mipping_;
    }

protected:
    const coords::chunkCoord coord_;
    const boost::scoped_ptr<dataInterface> chunkData_;
    const volume::volume * const vol_;

    chunkMipping mipping_;
};

} // namespace chunks
} // namespace om