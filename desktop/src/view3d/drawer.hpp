#pragma once

#include "mesh/omVolumeCuller.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentUtils.hpp"
#include "system/omAppState.hpp"
#include "system/omLocalPreferences.hpp"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "view3d/camera.h"
#include "view3d/gl.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"
#include "widgets/widgets.hpp"
#include "view3d/meshDrawInfo.hpp"

namespace om {
namespace v3d {

class Drawer {
 private:
  prefs::View3d& prefs_;
  Widgets& widgets_;
  DrawStatus& drawStatus_;
  Camera& camera_;
  OmViewGroupState& vgs_;
  OmSegmentation& vol_;

  std::vector<MeshDrawInfo*> drawInfos_;

 public:
  Drawer(prefs::View3d& prefs, Widgets& widgets, DrawStatus& drawStatus,
         Camera& camera, const std::vector<OmSegmentation*>& segmentations,
         OmViewGroupState& vgs)
      : prefs_(prefs),
        widgets_(widgets),
        drawStatus_(drawStatus),
        camera_(camera),
        vgs_(vgs),
        vol_(*vgs_.Segmentation().GetSegmentation()) {  // TODO: Fix this.
    for (auto* vol : segmentations) {
      drawInfos_.push_back(new MeshDrawInfo(*vol, vgs));
    }
  }

  ~Drawer() {
    for (auto* info : drawInfos_) {
      delete info;
    }
  }

  void PrimaryDraw() {
    drawStatus_.Reset();

    setupGL();
    {
      // draw selected and write to stencil
      // (for use with highlighting outline)
      glEnable(GL_STENCIL_TEST);
      glStencilFunc(GL_ALWAYS, 1, 0xFFFF);
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
      drawVolumes();
      glDisable(GL_STENCIL_TEST);

      widgets_.Draw();
    }
    teardownGL();
  }

  void DrawPickPoint() {
    setupGL();
    { drawVolumes(); }
    teardownGL();
  }

  void DrawChunkBoundaries() {
    // TODO: fix me!
    setupGL();
    {
      // walk through volume, and call drawClippedExtent....
    }
    teardownGL();
  }

 private:
  void setupGL() {
    // clear buffer
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // save the init modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // apply camera modelview matrix
    camera_.ApplyModelview();
  }

  void teardownGL() {
    // pop to init modelview
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }

  void drawVolumes() {
    // draw focus axis
    camera_.DrawFocusAxis();

    // setup culler to current projection-modelview matrix
    OmVolumeCuller mainCuller(
        camera_.GetProjModelViewMatrix(),
        coords::Norm(camera_.GetPosition(), vol_.Coords()),
        coords::Norm(camera_.GetFocus(), vol_.Coords()));

    // Draw meshes!
    for (auto* info : drawInfos_) {
      auto& vol = info->Vol();
      if (!vol.built()) {
        continue;
      }

      auto culler = mainCuller.GetTransformedCuller(
          vol.Coords().NormToGlobalMat(), vol.Coords().GlobalToNormMat());

      bool cullerChanged = info->DidCullerChange(culler);
      auto plan = info->GetPlan(*culler, cullerChanged);
      auto allowedDrawTimeMS = info->GetAllowedDrawTime();

      auto& drawer = info->Drawer();
      drawer.Draw(*plan, allowedDrawTimeMS);

      drawStatus_.Add(drawer.getPercDone());
    }
  }

  // draw chunk bounding box--broken? (purcaro)
  void drawClippedExtent(const coords::NormBbox& clippedNormExtent) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    // disable lighting for lines
    glDisable(GL_LIGHTING);

    // translate and scale to chunk norm extent
    const Vector3f translate = clippedNormExtent.getMin();
    const Vector3f scale =
        clippedNormExtent.getMax() - clippedNormExtent.getMin();

    // transform model view
    gl::glTranslatefv(translate.array);
    gl::glScalefv(scale.array);

    glTranslatef(0.5, 0.5, 0.5);
    glColor3f(0.5, 0.5, 0.5);
    gl::omglWireCube(1);
    glTranslatef(-0.5, -0.5, -0.5);

    // glScalefv( (Vector3f::ONE/scale).array);
    // glTranslatefv( (-translate).array);

    glPopMatrix();
    glPopAttrib();
  }
};
}
}  // om::v3d::
