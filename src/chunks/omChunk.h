#pragma once

/**
 *  OmChunk encapsulates a 3D-matrix of image data (typically 128^3) and
 *    its associated size/positioning metadata.
 *
 *  Brett Warne - bwarne@mit.edu - 2/24/09
 *  Michael Purcaro - purcaro@gmail.com - 1/29/11
 */

#include "chunks/omChunkMipping.hpp"
#include "volume/omVolumeTypes.hpp"

class channel;
class channelImpl;
class segmentation;
namespace om { namespace chunk { class dataInterface; } }

class OmChunk {
public:
    OmChunk(channel* vol, const om::chunkCoord& coord);
    OmChunk(channelImpl* vol, const om::chunkCoord& coord);
    OmChunk(segmentation* vol, const om::chunkCoord& coord);

    virtual ~OmChunk();

    bool ContainsVoxel(const om::dataCoord& vox) const {
        return vox.volume() == vol_ &&  
               vox.level() == GetLevel() &&
               mipping_.GetExtent().contains(vox);
    }

public:
    const om::chunkCoord& GetCoordinate() const {
        return coord_;
    }
    int GetLevel() const {
        return coord_.Level;
    }
    const om::dataCoord GetDimensions() const {
        return mipping_.GetExtent().getUnitDimensions();
    }

    om::chunk::dataInterface* Data(){
        return chunkData_.get();
    }

    OmChunkMipping& Mipping(){
        return mipping_;
    }

protected:
    const om::chunkCoord coord_;
    const boost::scoped_ptr<om::chunk::dataInterface> chunkData_;
    const mipVolume * const vol_;

    OmChunkMipping mipping_;
};

