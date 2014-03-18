#pragma once
#include "precomp.h"

#include "view3d.old/gl.h"
#include "common/common.h"
#include "common/logging.h"
#include "utility/glInclude.h"
#include "mesh/drawer/omMeshPlan.h"
#include "mesh/omMesh.h"
#include "mesh/omMeshManagers.hpp"
#include "mesh/omVolumeCuller.h"
#include "mesh/drawer/omSegmentPointers.h"
#include "segment/omSegmentUtils.hpp"
#include "events/events.h"
#include "system/omOpenGLGarbageCollector.hpp"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "utility/omTimer.hpp"
#include "view3d.old/om3dPreferences.hpp"
#include "viewGroup/omViewGroupState.h"
#include "chunks/omChunk.h"

class OmMeshDrawerImpl {
 private:
  OmSegmentation* const segmentation_;
  OmViewGroupState* const vgs_;
  OmSegments* const segments_;
  const OmBitfield drawOptions_;
  OmMeshPlan* const sortedSegments_;
  const float breakThreshold_;

  OmTimer elapsed_;

  bool redrawNeeded_;
  uint32_t numSegsDrawn_;
  uint64_t numVoxelsDrawn_;

  QGLContext const* context_;

 public:
  OmMeshDrawerImpl(OmSegmentation* seg, OmViewGroupState* vgs,
                   const OmBitfield drawOptions, OmMeshPlan* sortedSegments)
      : segmentation_(seg),
        vgs_(vgs),
        segments_(segmentation_->Segments()),
        drawOptions_(drawOptions),
        sortedSegments_(sortedSegments),
        breakThreshold_(vgs->getBreakThreshold()),
        redrawNeeded_(false),
        numSegsDrawn_(0),
        numVoxelsDrawn_(0),
        context_(QGLContext::currentContext()) {
    assert(context_ && "context should never be 0");
  }

  /**
   * Draw the mesh plan. Filters for relevant data values to be
   *   drawn depending on culler draw options
   */
  void Draw(const int allowedDrawTimeMS) {
    OmOpenGLGarbageCollector::CleanDisplayLists(context_);

    // transform to normal frame
    glPushMatrix();
    glMultMatrixf(segmentation_->Coords().NormToGlobalMat().ml);

    // draw volume axis
    if (checkDrawOption(DRAWOP_DRAW_VOLUME_AXIS)) {
      om::gl::old::glDrawPositiveAxis();
    }

    // return if no chunk level drawing
    if (!checkDrawOption(DRAWOP_LEVEL_CHUNKS)) {
      glPopMatrix();
      return;
    }

    if (sortedSegments_->empty()) {
      glPopMatrix();
      return;
    }

    glPushName(segmentation_->GetID());

    drawPlan(allowedDrawTimeMS);

    glPopName();

    glPopMatrix();

    if (redrawNeeded_) {
      om::event::Redraw3d();
    }
  }

  uint32_t NumSegsDrawn() const { return numSegsDrawn_; }

  uint32_t TotalNumSegmentsPresent() const { return sortedSegments_->size(); }

  uint64_t NumVoxelsDrawn() const { return numVoxelsDrawn_; }

  uint64_t TotalVoxelsPresent() const { return sortedSegments_->TotalVoxels(); }

  bool RedrawNeeded() const { return redrawNeeded_; }

 private:
  inline bool checkDrawOption(const OmBitfield option) {
    return drawOptions_ & option;
  }

  void drawPlan(const int allowedDrawTimeMS) {
    FOR_EACH(iter, *sortedSegments_) {

      if (elapsed_.ms_elapsed() > allowedDrawTimeMS) {
        redrawNeeded_ = true;
        return;
      }

      drawSegment(iter->first, iter->second);
    }
  }

  void drawSegment(OmSegment* seg, const om::coords::Chunk& coord) {
    OmMeshPtr mesh;
    segmentation_->MeshManagers().GetMesh(mesh, coord, seg->value(), 1);

    if (!mesh) {
      redrawNeeded_ = true;
      return;
    }

    if (!mesh->HasData()) {
      return;
    }

    ++numSegsDrawn_;
    numVoxelsDrawn_ += seg->size();

    // apply segment color
    colorMesh(seg);

    // draw mesh
    glPushName(seg->value());
    glPushName(OMGL_NAME_MESH);

    mesh->Draw(context_);

    glPopName();
    glPopName();
  }

  // draw chunk bounding box--broken? (purcaro)
  void drawClippedExtent(OmChunk* chunk) {
    return;  // FIXME!

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    // disable lighting for lines
    glDisable(GL_LIGHTING);

    const om::coords::NormBbox& clippedNormExtent =
        chunk->Mipping().GetClippedNormExtent();

    // translate and scale to chunk norm extent
    const Vector3f translate = clippedNormExtent.getMin();
    const Vector3f scale =
        clippedNormExtent.getMax() - clippedNormExtent.getMin();

    // transform model view
    om::gl::old::glTranslatefv(translate.array);
    om::gl::old::glScalefv(scale.array);

    glTranslatef(0.5, 0.5, 0.5);
    glColor3f(0.5, 0.5, 0.5);
    // omglWireCube(1);
    glTranslatef(-0.5, -0.5, -0.5);

    // glScalefv( (Vector3f::ONE/scale).array);
    // glTranslatefv( (-translate).array);

    glPopMatrix();
    glPopAttrib();
  }

  inline void colorMesh(OmSegment* segment) {
    om::segment::coloring sccType;

    if (vgs_->shouldVolumeBeShownBroken()) {
      sccType = om::segment::coloring::SEGMENTATION_BREAK_BLACK;
    } else {
      sccType = om::segment::coloring::SEGMENTATION;
    }

    applyColor(segment, sccType);
  }

  void applyColor(OmSegment* seg, const om::segment::coloring sccType) {
    if (seg->getParent() &&
        sccType != om::segment::coloring::SEGMENTATION_BREAK_BLACK) {
      applyColor(segments_->findRoot(seg), sccType);
      return;
    }

    Vector3f hyperColor;
    if (om::segment::coloring::SEGMENTATION_BREAK_BLACK != sccType) {
      hyperColor = seg->GetColorFloat() * 2.;
    } else {

      if (!qFuzzyCompare(1, vgs_->getBreakThreshold())) {
        if (OmSegmentUtils::UseParentColorBasedOnThreshold(seg,
                                                           breakThreshold_)) {
          // WARNING: recusive operation is O(depth of MST)

          OmSegment* segToShow =
              OmSegmentUtils::GetSegmentFromThreshold(seg, breakThreshold_);
          applyColor(segToShow, sccType);
          return;
        }
      }

      hyperColor = seg->GetColorFloat();
    }

    // check coloring options
    if (checkDrawOption(DRAWOP_SEGMENT_COLOR_HIGHLIGHT)) {
      glColor3fv(OmPreferences::GetVector3f(om::PREF_VIEW3D_HIGHLIGHT_COLOR_V3F)
                     .array);

    } else if (checkDrawOption(DRAWOP_SEGMENT_COLOR_TRANSPARENT)) {
      om::gl::old::glColor3fva(
          hyperColor.array,
          OmPreferences::GetFloat(om::PREF_VIEW3D_TRANSPARENT_ALPHA_FLT));

    } else if (Om3dPreferences::getDoDiscoBall()) {
      showAsDiscoBall(hyperColor);

    } else {
      glColor3fv(hyperColor.array);
    }
  }

  inline void showAsDiscoBall(const Vector3f& hyperColor) {
    static float s = 10.0;
    static float dir = 1;

    glEnable(GL_BLEND);
    om::gl::old::glColor3fva(hyperColor.array, s / 200. + .4);

    s += .1 * dir;

    if (s > 60) {
      dir = -1.;
    }

    if (s < 10) {
      dir = 1.;
    }

    glMaterialf(GL_FRONT, GL_SHININESS, 100 - s);
  }
};
