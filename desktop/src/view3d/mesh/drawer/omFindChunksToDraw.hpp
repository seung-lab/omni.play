#pragma once

#include "coordinates/volumeSystem.h"
#include "mesh/omVolumeCuller.h"

namespace om {
namespace v3d {

struct CoordAndDistance {
  om::coords::Chunk coord;
  float distance;
};
typedef std::deque<CoordAndDistance> drawChunks_t;

class FindChunksToDraw {
 private:
  const om::coords::VolumeSystem& system_;

  struct stackEntry {
    om::coords::Chunk cc;
    bool testVis;
  };
  std::vector<stackEntry> stack_;
  drawChunks_t& chunksToDraw_;

 public:
  FindChunksToDraw(const om::coords::VolumeSystem& coords,
                   drawChunks_t& chunksToDraw)
      : system_(coords), chunksToDraw_(chunksToDraw) {
    stack_.reserve(system_.ComputeTotalNumChunks());
  }

  void Find(const OmVolumeCuller& culler) {
    stack_.push_back({system_.RootMipChunkCoordinate(), true});

    while (!stack_.empty()) {
      auto se = stack_.back();
      stack_.pop_back();
      determineChunksToDraw(culler, se.cc, se.testVis);
    }
  }

 private:
  /**
   * Recursively draw MipChunks within the Segmentation using the
   *  MipCoordinate hierarchy. Uses the OmVolumeCuller to determine
   *  the visibility of a MipChunk.  If visible, the MipChunk is either
   *  drawn or the recursive draw process is called on its children.
   */
  inline void determineChunksToDraw(const OmVolumeCuller& culler,
                                    const om::coords::Chunk& cc, bool testVis) {
    if (!system_.ContainsMipChunk(cc)) {
      return;
    }

    if (testVis) {
      // check if frustum contains chunk
      const auto bounds = cc.BoundingBox(system_).ToNormBbox();

      switch (culler.TestChunk(bounds)) {
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

    auto d = shouldChunkBeDrawn(culler, cc);
    if (d.shouldDraw) {
      chunksToDraw_.push_back({cc, d.distance});

    } else {
      const auto children = cc.ChildrenCoords();
      for (auto& child_cc : children) {
        stack_.push_back({child_cc, testVis});
      }
    }
  }

  struct distanceRet {
    bool shouldDraw;
    float distance;
  };

  /**
   * Given that the chunk is visible, determine if it should be drawn
   * or if we should continue refining so as to draw children.
   */
  inline distanceRet shouldChunkBeDrawn(const OmVolumeCuller& culler,
                                        const om::coords::Chunk& chunk) {
    // draw if MIP 0
    if (0 == chunk.mipLevel()) {
      return {true, 0};
    }

    const auto normExtent = chunk.BoundingBox(system_).ToNormBbox();
    const auto center = normExtent.getCenter();
    const auto camera = culler.GetPosition();

    const float camera_to_center = center.distance(camera);
    const float distance = (normExtent.getMax() - center).length();

    // if distance too large, just draw it - else keep breaking it down
    return {camera_to_center > distance, distance};
  }
};
}
}  // om::v3d::
