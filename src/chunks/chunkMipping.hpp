#pragma once

#include "volume/mipVolume.h"

class chunkMipping {
private:
    //octree properties
    om::dataBbox dataExtent_;
    om::normBbox normExtent_; // extent of chunk in norm space
    om::normBbox clippedNormExtent_; // extent of contained data in norm space
    coords::chunkCoord coord_;
    std::set<coords::chunkCoord> childrenCoordinates_;

    uint32_t numVoxels_;
    uint32_t numBytes_;

public:
    chunkMipping(mipVolume* vol, const coords::chunkCoord& coord)
        : dataExtent_(coord.chunkBoundingBox(vol))
        , normExtent_(dataExtent_.toNormBbox())
        , clippedNormExtent_(dataExtent_.toNormBbox())
        , coord_(coord)
        , numVoxels_(vol->Coords().GetNumberOfVoxelsPerChunk())
        , numBytes_(numVoxels_ * vol->GetBytesPerVoxel())
    {
        //set children
        vol->Coords().ValidMipChunkCoordChildren(coord, childrenCoordinates_);

        clippedNormExtent_.intersect(AxisAlignedBoundingBox<float>::UNITBOX);
    }

    inline const om::dataBbox& GetExtent() const {
        return dataExtent_;
    }

    inline const om::normBbox& GetNormExtent() const {
        return normExtent_;
    }

    inline const om::normBbox& GetClippedNormExtent() const {
        return clippedNormExtent_;
    }

    inline const std::set<coords::chunkCoord>& GetChildrenCoordinates() const {
        return childrenCoordinates_;
    }

    inline uint32_t NumVoxels() const {
        return numVoxels_;
    }

    inline uint32_t NumBytes() const {
        return numBytes_;
    }
};

