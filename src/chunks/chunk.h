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

class channel;
class channelImpl;
class segmentation;
namespace om { namespace chunk { class dataInterface; } }

class chunk {
public:
    chunk(channel* vol, const coords::chunkCoord& coord);
    chunk(channelImpl* vol, const coords::chunkCoord& coord);
    chunk(segmentation* vol, const coords::chunkCoord& coord);

    virtual ~chunk();

    bool ContainsVoxel(const coords::dataCoord& vox) const {
        return vox.volume() == vol_ &&  
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

    om::chunk::dataInterface* Data(){
        return chunkData_.get();
    }

    chunkMipping& Mipping(){
        return mipping_;
    }

protected:
    const coords::chunkCoord coord_;
    const boost::scoped_ptr<om::chunk::dataInterface> chunkData_;
    const mipVolume * const vol_;

    chunkMipping mipping_;
};

