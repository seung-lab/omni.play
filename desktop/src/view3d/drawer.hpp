#pragma once
#include "precomp.h"

#include "vmmlib/vmmlib.h"
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
  Widgets& widgets_;
  DrawStatus& drawStatus_;
  Camera& camera_;
  const om::coords::VolumeSystem& system_;
  std::vector<std::unique_ptr<MeshDrawInfo>> drawInfos_;

 public:
  Drawer(Widgets& widgets, DrawStatus& drawStatus,
         Camera& camera, const std::vector<OmSegmentation*>& segmentations,
         OmViewGroupState& vgs, const om::coords::VolumeSystem& system)
      : widgets_(widgets),
        drawStatus_(drawStatus),
        camera_(camera),
        system_(system) {
    for (auto* vol : segmentations) {
      drawInfos_.emplace_back(std::make_unique<MeshDrawInfo>(*vol, vgs));
    }
  }

  ~Drawer() {}

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

  void DrawSegmentationBoundaries(std::vector<OmSegmentation*> segmentations) {
    for (auto segmentation : segmentations) {
       auto bounds = segmentation->Coords().Bounds();
       auto globalBbox = bounds.ToGlobalBbox();
       drawClippedExtent(globalBbox);
    }
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
    om::v3d::VolumeCuller mainCuller(camera_.GetProjModelViewMatrix(),
                              om::coords::Norm(camera_.GetPosition(), system_),
                              om::coords::Norm(camera_.GetFocus(), system_));

    // Draw meshes!
    for (auto& info : drawInfos_) {
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

  void drawClippedExtent(const vmml::AxisAlignedBoundingBox<float>& extent) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    // disable lighting for lines
    glDisable(GL_LIGHTING);

    // translate and scale to chunk norm extent
    const Vector3f translate = extent.getMin();
    const Vector3f scale =
        extent.getMax() - extent.getMin();

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
