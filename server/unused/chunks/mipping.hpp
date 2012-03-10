#pragma once

#include "volume/volume.h"

namespace om {
namespace chunks {

class mipping {
private:
    const volume::volume* vol_;

    //octree properties
    coords::dataBbox dataExtent_;
    coords::normBbox normExtent_; // extent of chunk in norm space
    coords::normBbox clippedNormExtent_; // extent of contained data in norm space
    coords::chunkCoord coord_;
    std::set<coords::chunkCoord> childrenCoordinates_;

    uint32_t numVoxels_;
    uint32_t numBytes_;

public:
    mipping(volume::volume* vol, const coords::chunkCoord& coord)
        : vol_(vol)
        , dataExtent_(coord.chunkBoundingBox(&vol->CoordinateSystem()))
        , normExtent_(dataExtent_.toNormBbox())
        , clippedNormExtent_(dataExtent_.toNormBbox())
        , coord_(coord)
        , numBytes_(numVoxels_ * vol->GetBytesPerVoxel())
    {
        //set children
        validMipChunkCoordChildren(coord, childrenCoordinates_);

        const Vector3i dims = vol->CoordinateSystem().GetChunkDimensions();
        numVoxels_ = dims.x * dims.y * dims.z;

        clippedNormExtent_.intersect(AxisAlignedBoundingBox<float>::UNITBOX);
    }

    inline const coords::dataBbox& GetExtent() const {
        return dataExtent_;
    }

    inline const coords::normBbox& GetNormExtent() const {
        return normExtent_;
    }

    inline const coords::normBbox& GetClippedNormExtent() const {
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

private:
    inline void validMipChunkCoordChildren(const coords::chunkCoord & rMipCoord,
                                           std::set<coords::chunkCoord>& children) const
    {
        //clear set
        children.clear();

        //get all possible children
        coords::chunkCoord possible_children[8];
        rMipCoord.ChildrenCoords(possible_children);

        //for all possible children
        for (int i = 0; i < 8; i++) {
            if (vol_->CoordinateSystem().ContainsMipChunkCoord(possible_children[i])) {
                children.insert(possible_children[i]);
            }
        }
    }
};

} // namespace volume
} // namespace om
