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

class OmChannel;
class OmChannelImpl;
class OmSegmentation;
namespace om { namespace chunk { class dataInterface; } }

class OmChunk {
public:
    OmChunk(OmChannel* vol, const om::coords::Chunk& coord);
    OmChunk(OmChannelImpl* vol, const om::coords::Chunk& coord);
    OmChunk(OmSegmentation* vol, const om::coords::Chunk& coord);

    virtual ~OmChunk();

    bool ContainsVoxel(const om::coords::Data& vox) const {
        return vox.volume() == vol_ &&  
               vox.level() == GetLevel() &&
               mipping_.GetExtent().contains(vox);
    }

public:
    const om::coords::Chunk& GetCoordinate() const {
        return coord_;
    }
    int GetLevel() const {
        return coord_.Level;
    }
    const om::coords::Data GetDimensions() const {
        return mipping_.GetExtent().getUnitDimensions();
    }

    om::chunk::dataInterface* Data(){
        return chunkData_.get();
    }

    OmChunkMipping& Mipping(){
        return mipping_;
    }

protected:
    const om::coords::Chunk coord_;
    const boost::scoped_ptr<om::chunk::dataInterface> chunkData_;
    const OmMipVolume * const vol_;

    OmChunkMipping mipping_;
};

