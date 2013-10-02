#pragma once

#include "events/events.h"
#include "mesh/drawer/omMeshPlan.h"
#include "mesh/drawer/percDone.hpp"
#include "mesh/omVolumeCuller.h"
#include "segment/omSegmentPointers.h"
#include "system/omOpenGLGarbageCollector.hpp"
#include "utility/timer.hpp"
#include "view3d/gl.h"
#include "viewGroup/omViewGroupState.h"
#include "mesh/displayListCachedDataSource.hpp"

#include <QGLContext>

namespace om {
namespace v3d {

class DrawerImpl {
 public:
  DrawerImpl(mesh::DisplayListCachedDataSource& meshes,
             const coords::VolumeSystem& system,
             const common::ID segmentationID)
      : meshes_(meshes),
        system_(system),
        segmentationID_(segmentationID),
        context_(QGLContext::currentContext()) {
    // if(!context_) {
    //   throw VerifyException("QGLContext should never be 0");
    // }
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
  void drawSegment(const common::SegID segID, const coords::Chunk& coord,
                   const Vector3f& color) {

    mesh::DisplayListMeshCoord dlc = {coords::Mesh(coord, segID), context_};

    auto mesh = meshes_.Get(dlc, true);

    if (!mesh) {
      perc_done_.missingMesh();
      return;
    }

    // apply segment color
    glColor3fv(color.array);

    // draw mesh
    glPushName(segID);
    mesh->Draw();
    glPopName();

    perc_done_.justDrew(mesh);
  }

 private:
  mesh::DisplayListCachedDataSource& meshes_;
  const coords::VolumeSystem& system_;
  const common::ID segmentationID_;
  QGLContext const* const context_;

  v3d::PercDone perc_done_;
};
}
}  // om::v3d::
