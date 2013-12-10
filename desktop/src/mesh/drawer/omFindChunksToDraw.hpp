#pragma once

#include "chunks/omSegChunk.h"
#include "volume/omSegmentation.h"

class OmFindChunksToDraw {
 private:
  OmSegmentation* const segmentation_;
  OmVolumeCuller* const culler_;

  std::shared_ptr<std::deque<OmSegChunk*> > chunksToDraw_;

 public:
  OmFindChunksToDraw(OmSegmentation* segmentation, OmVolumeCuller* culler)
      : segmentation_(segmentation), culler_(culler) {}

  std::shared_ptr<std::deque<OmSegChunk*> > FindChunksToDraw() {
    chunksToDraw_ = std::make_shared<std::deque<OmSegChunk*> >();
    determineChunksToDraw(segmentation_->Coords().RootMipChunkCoordinate(),
                          true);
    return chunksToDraw_;
  }

 private:
  /**
   * Recursively draw MipChunks within the Segmentation using the
   *  MipCoordinate hierarchy. Uses the OmVolumeCuller to determine
   *  the visibility of a MipChunk.  If visible, the MipChunk is either
   *  drawn or the recursive draw process is called on its children.
   */
  void determineChunksToDraw(const om::coords::Chunk& chunkCoord,
                             bool testVis) {
    OmSegChunk* chunk = segmentation_->GetChunk(chunkCoord);

    if (testVis) {
      // check if frustum contains chunk
      switch (culler_->TestChunk(chunk->Mipping().GetNormExtent())) {
        case VISIBILITY_NONE:
          return;

        case VISIBILITY_PARTIAL:
          // continue drawing tree and continue testing children
          break;

        case VISIBILITY_FULL:
          // continue drawing tree, but assume children are visible
          testVis = false;
          break;
      }
    }

    if (shouldChunkBeDrawn(chunk)) {
      chunksToDraw_->push_back(chunk);

    } else {
      FOR_EACH(iter, chunk->Mipping().GetChildrenCoordinates()) {
        determineChunksToDraw(*iter, testVis);
      }
    }
  }

  /**
   *  Given that the chunk is visible, determine if it should be drawn
   *  or if we should continue refining so as to draw children.
   */
  bool shouldChunkBeDrawn(OmSegChunk* chunk) {
    // draw if MIP 0
    if (0 == chunk->GetCoordinate().mipLevel()) {
      return true;
    }

    const om::coords::NormBbox& normExtent = chunk->Mipping().GetNormExtent();
    const om::coords::NormBbox& clippedNormExtent =
        chunk->Mipping().GetClippedNormExtent();

    const om::coords::Norm camera = culler_->GetPosition();
    const om::coords::Norm center = clippedNormExtent.getCenter();

    const float camera_to_center = center.distance(camera);
    const float distance =
        (normExtent.getMax() - normExtent.getCenter()).length();

    // if distance too large, just draw it - else keep breaking it down
    return (camera_to_center > distance);
  }
};
