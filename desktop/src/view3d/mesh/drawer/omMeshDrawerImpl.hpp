#pragma once
#include "precomp.h"

#include "events/events.h"
#include "view3d/mesh/drawer/omMeshPlan.h"
#include "view3d/mesh/drawer/percDone.hpp"
#include "view3d/mesh/omVolumeCuller.h"
#include "system/omOpenGLGarbageCollector.hpp"
#include "utility/timer.hpp"
#include "view3d/gl.h"
#include "viewGroup/omViewGroupState.h"
#include "view3d/mesh/cacheWrapper.hpp"

namespace om {
namespace v3d {

class DrawerImpl {
 public:
  DrawerImpl(const om::coords::VolumeSystem& system,
             const common::ID segmentationID)
      : system_(system),
        segmentationID_(segmentationID),
        context_(nullptr),
        meshes_(segmentationID) {}

  /**
   * Draw the mesh plan. Filters for relevant data values to be
   *   drawn depending on culler draw options
   */
  void Draw(const MeshPlan& meshPlan, const int allowedDrawTimeMS) {
    if (!context_) {
      context_ = QGLContext::currentContext();
    }
    perc_done_.Reset(meshPlan.size());
    utility::timer elapsed;

    OmOpenGLGarbageCollector::CleanDisplayLists(context_);

    if (meshPlan.empty()) {
      if (!meshPlan.HasAllSegments()) {
        event::Redraw3d();
      }
      return;
    }

    // transform to normal frame
    glPushMatrix();
    glMultMatrixf(system_.NormToGlobalMat().ml);
    glPushName(segmentationID_);

    // Step 1: Find segment IDs with one or more chunks that are not completely loaded, yet. If found,
    //         render the least detailed (highest MIP) version for this (covers all children, loaded or not).
    //         This ensures a somewhat useful image while saving most of the time for loading missing segments.
    om::common::SegIDUnorderedSet unfinishedSegments;
    for (auto& kv : meshPlan) {
      auto mesh = meshes_.Get(kv.second.coord, kv.first.seg->value());
      if (!mesh || !mesh->ReadyForDrawing()) {
        if (unfinishedSegments.find(kv.first.seg->value()) == unfinishedSegments.end()) {
          unfinishedSegments.emplace(kv.first.seg->value());
          drawSegment(kv.first.seg->value(), system_.RootMipChunkCoordinate(), kv.second.color);
        }
      }
    }

    // Step 2: Render all the segment-chunks that are already on GPU side. Assuming that low FPS is preferable
    //         to an incomplete scene.
    for (auto& kv : meshPlan) {
      if (unfinishedSegments.find(kv.first.seg->value()) == unfinishedSegments.end()) {
        drawSegment(kv.first.seg->value(), kv.second.coord, kv.second.color);
      }
    }

    // Step 3: If there is still time left, prepare remaining segment-chunks (but at least one) for the next frame.
    for (auto& kv : meshPlan) {     
      if (unfinishedSegments.find(kv.first.seg->value()) != unfinishedSegments.end()) {
        auto mesh = meshes_.Get(kv.second.coord, kv.first.seg->value());
        if (mesh && !mesh->ReadyForDrawing()) {
          mesh->PrepareDraw(context_);
          if (elapsed.ms_elapsed() > allowedDrawTimeMS) {
            break;
          }
        }
      }
    }

    glPopName();
    glPopMatrix();

    if (perc_done_.needsRedraw() || perc_done_.percentDone() < 1.0
     || !meshPlan.HasAllSegments()) {
      event::Redraw3d();
    }
  }

  const v3d::PercDone& getPercDone() const { return perc_done_; }

 private:
  bool drawSegment(const common::SegID segID, const om::coords::Chunk& coord,
                   const Vector3f& color) {

    auto mesh = meshes_.Get(coord, segID);

    if (!mesh) {
      perc_done_.missingMesh();
      return false;
    }

    // apply segment color
    glColor3fv(color.array);

    // draw mesh
    glPushName(segID);
    mesh->Draw(context_);
    glPopName();

    perc_done_.justDrew(mesh);
    return true;
  }

 private:
  // mesh::DisplayListCachedDataSource& meshes_;
  const om::coords::VolumeSystem& system_;
  const common::ID segmentationID_;
  const QGLContext* context_;
  v3d::PercDone perc_done_;
  om::v3d::mesh::CacheWrapper meshes_;
};
}
}  // om::v3d::
