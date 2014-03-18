#pragma once
#include "precomp.h"

#include "volume/omMipVolume.h"

class OmChunkMipping {
 private:
  // octree properties
  om::coords::DataBbox dataExtent_;
  om::coords::NormBbox normExtent_;         // extent of chunk in norm space
  om::coords::NormBbox clippedNormExtent_;  // extent of contained data in norm
                                            // space
  om::coords::Chunk coord_;
  std::set<om::coords::Chunk> childrenCoordinates_;

  uint32_t numVoxels_;
  uint32_t numBytes_;

 public:
  OmChunkMipping(OmMipVolume* vol, const om::coords::Chunk& coord)
      : dataExtent_(coord.BoundingBox(vol->Coords())),
        normExtent_(dataExtent_.ToNormBbox()),
        clippedNormExtent_(dataExtent_.ToNormBbox()),
        coord_(coord),
        numVoxels_(vol->Coords().GetNumberOfVoxelsPerChunk()),
        numBytes_(numVoxels_ * vol->GetBytesPerVoxel()) {
    // set children
    vol->Coords().ValidMipChunkCoordChildren(coord, childrenCoordinates_);

    clippedNormExtent_.intersect(AxisAlignedBoundingBox<float>::UNITBOX);
  }

  inline const om::coords::DataBbox& Extent() const { return dataExtent_; }

  inline const om::coords::NormBbox& GetNormExtent() const {
    return normExtent_;
  }

  inline const om::coords::NormBbox& GetClippedNormExtent() const {
    return clippedNormExtent_;
  }

  inline const std::set<om::coords::Chunk>& GetChildrenCoordinates() const {
    return childrenCoordinates_;
  }

  inline uint32_t NumVoxels() const { return numVoxels_; }

  inline uint32_t NumBytes() const { return numBytes_; }
};
