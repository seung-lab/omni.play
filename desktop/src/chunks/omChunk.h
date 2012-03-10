#pragma once

/**
 *  OmChunk encapsulates a 3D-matrix of image data (typically 128^3) and
 *    its associated size/positioning metadata.
 *
 *  Brett Warne - bwarne@mit.edu - 2/24/09
 *  Michael Purcaro - purcaro@gmail.com - 1/29/11
 */

#include "chunks/omChunkCoord.h"
#include "chunks/omChunkMipping.hpp"
#include "volume/omVolumeTypes.hpp"

class OmChannel;
class OmChannelImpl;
class OmSegmentation;
namespace om { namespace chunk { class dataInterface; } }

class OmChunk {
public:
    OmChunk(OmChannel* vol, const OmChunkCoord& coord);
    OmChunk(OmChannelImpl* vol, const OmChunkCoord& coord);
    OmChunk(OmSegmentation* vol, const OmChunkCoord& coord);

    virtual ~OmChunk();

    bool ContainsVoxel(const DataCoord& vox) const {
        return mipping_.GetExtent().contains(vox);
    }

public:
    const OmChunkCoord& GetCoordinate() const {
        return coord_;
    }
    int GetLevel() const {
        return coord_.Level;
    }
    const Vector3i GetDimensions() const {
        return mipping_.GetExtent().getUnitDimensions();
    }

    om::chunk::dataInterface* Data(){
        return chunkData_.get();
    }

    OmChunkMipping& Mipping(){
        return mipping_;
    }

protected:
    const OmChunkCoord coord_;
    const boost::scoped_ptr<om::chunk::dataInterface> chunkData_;

    OmChunkMipping mipping_;
};

