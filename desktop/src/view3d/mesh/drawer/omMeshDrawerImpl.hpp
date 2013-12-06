#pragma once

#include "events/events.h"
#include "view3d/mesh/drawer/omMeshPlan.h"
#include "view3d/mesh/drawer/percDone.hpp"
#include "view3d/mesh/omVolumeCuller.h"
#include "segment/omSegmentPointers.h"
#include "system/omOpenGLGarbageCollector.hpp"
#include "utility/timer.hpp"
#include "view3d/gl.h"
#include "viewGroup/omViewGroupState.h"
#include "view3d/mesh/cacheWrapper.hpp"

#include <QGLContext>

namespace om {
namespace v3d {

class DrawerImpl {
 public:
  DrawerImpl(const OmMipVolCoords& system, const common::ID segmentationID)
      : system_(system),
        segmentationID_(segmentationID),
        context_(QGLContext::currentContext()),
        meshes_(segmentationID) {
    if (!context_) {
      log_infos << "WARNING: QLContext was 0" << std::endl;
      //   throw VerifyException("QGLContext should never be 0");
    }
  }

  /**
   * Draw the mesh plan. Filters for relevant data values to be
   *   drawn depending on culler draw options
   */
  void Draw(const MeshPlan& meshPlan, const int allowedDrawTimeMS) {
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

    for (auto& kv : meshPlan) {
      if (elapsed.ms_elapsed() > allowedDrawTimeMS) {
        break;
      }
      drawSegment(kv.first.seg->value(), kv.second.coord, kv.second.color);
    }

    glPopName();
    glPopMatrix();

    if (perc_done_.needsRedraw()) {
      event::Redraw3d();
    }
  }

  const v3d::PercDone& getPercDone() const { return perc_done_; }

 private:
  void drawSegment(const common::SegID segID, const om::chunkCoord& coord,
                   const Vector3f& color) {

    auto mesh = meshes_.Get(coord, segID);

    if (!mesh) {
      perc_done_.missingMesh();
      return;
    }

    // apply segment color
    glColor3fv(color.array);

    // draw mesh
    glPushName(segID);
    mesh->Draw(context_);
    glPopName();

    perc_done_.justDrew(mesh);
  }

 private:
  // mesh::DisplayListCachedDataSource& meshes_;
  const OmMipVolCoords& system_;
  const common::ID segmentationID_;
  QGLContext const* const context_;
  v3d::PercDone perc_done_;
  om::v3d::mesh::CacheWrapper meshes_;
};
}
}  // om::v3d::
