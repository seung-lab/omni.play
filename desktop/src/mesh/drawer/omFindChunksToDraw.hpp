#pragma once
#include "precomp.h"

#include "volume/omSegmentation.h"

class OmFindChunksToDraw {
 private:
  OmSegmentation* const segmentation_;
  OmVolumeCuller* const culler_;

 public:
  OmFindChunksToDraw(OmSegmentation* segmentation, OmVolumeCuller* culler)
      : segmentation_(segmentation), culler_(culler) {}

  std::shared_ptr<std::deque<om::coords::Chunk>> FindChunksToDraw() {
    return determineChunksToDraw(
        segmentation_->Coords().RootMipChunkCoordinate(), true);
  }

 private:
  /**
   * Recursively draw MipChunks within the Segmentation using the
   *  MipCoordinate hierarchy. Uses the OmVolumeCuller to determine
   *  the visibility of a MipChunk.  If visible, the MipChunk is either
   *  drawn or the recursive draw process is called on its children.
   */
  std::shared_ptr<std::deque<om::coords::Chunk>> determineChunksToDraw(
      const om::coords::Chunk& chunkCoord, bool testVis) {
    std::shared_ptr<std::deque<om::coords::Chunk>> ret(
        new std::deque<om::coords::Chunk>());

    OmChunkMipping mipping(segmentation_, chunkCoord);

    if (testVis) {
      // check if frustum contains chunk
      switch (culler_->TestChunk(mipping.GetNormExtent())) {
        case VISIBILITY_NONE:
          return ret;

        case VISIBILITY_PARTIAL:
          // continue drawing tree and continue testing children
          break;

        case VISIBILITY_FULL:
          // continue drawing tree, but assume children are visible
          testVis = false;
          break;
      }
    }

    if (shouldChunkBeDrawn(chunkCoord, mipping)) {
      ret->push_back(chunkCoord);

    } else {
      FOR_EACH(iter, mipping.GetChildrenCoordinates()) {
        determineChunksToDraw(*iter, testVis);
      }
    }

    return ret;
  }

  /**
   *  Given that the chunk is visible, determine if it should be drawn
   *  or if we should continue refining so as to draw children.
   */
  bool shouldChunkBeDrawn(const om::coords::Chunk& chunk,
                          const OmChunkMipping& mipping) {
    // draw if MIP 0
    if (0 == chunk.mipLevel()) {
      return true;
    }

    const om::coords::NormBbox& normExtent = mipping.GetNormExtent();
    const om::coords::NormBbox& clippedNormExtent =
        mipping.GetClippedNormExtent();

    const om::coords::Norm camera = culler_->GetPosition();
    const om::coords::Norm center = clippedNormExtent.getCenter();

    const float camera_to_center = center.distance(camera);
    const float distance =
        (normExtent.getMax() - normExtent.getCenter()).length();

    // if distance too large, just draw it - else keep breaking it down
    return (camera_to_center > distance);
  }
};
