#pragma once

#include "volume/omMipVolume.h"

class OmChunkMipping {
private:
    //octree properties
    om::dataBbox dataExtent_;
    om::normBbox normExtent_; // extent of chunk in norm space
    om::normBbox clippedNormExtent_; // extent of contained data in norm space
    om::chunkCoord coord_;
    std::set<om::chunkCoord> childrenCoordinates_;

    uint32_t numVoxels_;
    uint32_t numBytes_;

public:
    OmChunkMipping(OmMipVolume* vol, const om::chunkCoord& coord)
        : dataExtent_(coord.BoundingBox(vol))
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

    inline const std::set<om::chunkCoord>& GetChildrenCoordinates() const {
        return childrenCoordinates_;
    }

    inline uint32_t NumVoxels() const {
        return numVoxels_;
    }

    inline uint32_t NumBytes() const {
        return numBytes_;
    }
};

