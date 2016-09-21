#include "common/common.h"
#include "view3d/gl.h"
#include "view3d/view3d.h"
#include "viewBoxWidget.h"
#include "system/omLocalPreferences.hpp"
#include "system/omStateManager.h"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "utility/channelDataWrapper.hpp"
#include "view3d/3dPreferences.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "zi/utility.h"

using namespace om::v3d;

ViewBoxWidget::ViewBoxWidget(View3d& view3d, OmViewGroupState& vgs)
    : Widget(view3d), vgs_(vgs) {}

/**
 *  Draw the three orthogonal slices from of the view box
 */
void ViewBoxWidget::Draw() {
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_LIGHTING);

  // set line width
  static const int RECT_WIREFRAME_LINE_WIDTH = 2;
  glLineWidth(RECT_WIREFRAME_LINE_WIDTH);

  auto& view2dState = vgs_.View2dState();

  if (Om3dPreferences::get2DViewFrameIn3D()) {
    draw2dBoxWrapper(view2dState, om::common::ViewType::XY_VIEW);
    draw2dBoxWrapper(view2dState, om::common::ViewType::XZ_VIEW);
    draw2dBoxWrapper(view2dState, om::common::ViewType::ZY_VIEW);
  }

  if (Om3dPreferences::getDrawCrosshairsIn3D()) {
    drawLines(view2dState.GetScaledSliceDepth());
  }
  glPopAttrib();

  if (Om3dPreferences::getDrawSegmentationBoundingBox()) {
    view3d_.DrawSegmentationBoundaries();
  }
}

void ViewBoxWidget::draw2dBoxWrapper(OmViewGroupView2dState& view2dState,
                                     const om::common::ViewType viewType) {
  //    drawChannelData(viewType, OmStateManager::GetViewDrawable(viewType));
  draw2dBox(viewType, view2dState.GetViewSliceMin(viewType),
            view2dState.GetViewSliceMax(viewType),
            view2dState.GetScaledSliceDepth(viewType));
}

/**
 *  Draw a rectangle given the verticies in counter-clockwise order
 */
void ViewBoxWidget::drawRectangle(const Vector3i& v0, const Vector3i& v1,
                                  const Vector3i& v2, const Vector3i& v3) {
  glBegin(GL_LINE_STRIP);
  glVertex3iv(v0.array);
  glVertex3iv(v1.array);
  glVertex3iv(v2.array);
  glVertex3iv(v3.array);
  glVertex3iv(v0.array);
  glEnd();
}

void ViewBoxWidget::drawLines(Vector3i depth) {
  Vector3i v0, v1;

  float distance = ((float)Om3dPreferences::getCrosshairValue()) / 10.0;

  glColor3fv(om::gl::OMGL_BLUE);
  v0 = Vector3i(depth.x, depth.y, depth.z - distance);
  v1 = Vector3i(depth.x, depth.y, depth.z + distance);
  glBegin(GL_LINE_STRIP);
  glVertex3iv(v0.array);
  glVertex3iv(v1.array);
  glEnd();

  glColor3fv(om::gl::OMGL_GREEN);
  v0 = Vector3i(depth.x, depth.y - distance, depth.z);
  v1 = Vector3i(depth.x, depth.y + distance, depth.z);
  glBegin(GL_LINE_STRIP);
  glVertex3iv(v0.array);
  glVertex3iv(v1.array);
  glEnd();

  glColor3fv(om::gl::OMGL_RED);
  v0 = Vector3i(depth.x - distance, depth.y, depth.z);
  v1 = Vector3i(depth.x + distance, depth.y, depth.z);
  glBegin(GL_LINE_STRIP);
  glVertex3iv(v0.array);
  glVertex3iv(v1.array);
  glEnd();
}

/**
 *  Draw a given orthogonal slice of a bbox given the plane and offset of plane
 */
void ViewBoxWidget::draw2dBox(const om::common::ViewType plane,
                              const Vector2f& min, const Vector2f& max,
                              const float depth) {
  Vector3i v0, v1, v2, v3;

  switch (plane) {
    case om::common::ViewType::XY_VIEW:
      glColor3fv(om::gl::OMGL_BLUE);
      v0 = Vector3i(min.x, min.y, depth);
      v1 = Vector3i(max.x, min.y, depth);
      v2 = Vector3i(max.x, max.y, depth);
      v3 = Vector3i(min.x, max.y, depth);
      break;

    case om::common::ViewType::XZ_VIEW:
      glColor3fv(om::gl::OMGL_GREEN);
      v0 = Vector3i(min.x, depth, min.y);
      v1 = Vector3i(min.x, depth, max.y);
      v2 = Vector3i(max.x, depth, max.y);
      v3 = Vector3i(max.x, depth, min.y);
      break;

    case om::common::ViewType::ZY_VIEW:
      glColor3fv(om::gl::OMGL_RED);
      v0 = Vector3i(depth, min.y, min.x);
      v1 = Vector3i(depth, max.y, min.x);
      v2 = Vector3i(depth, max.y, max.x);
      v3 = Vector3i(depth, min.y, max.x);
      break;
  }

  drawRectangle(v0, v1, v2, v3);
}

// void ViewBoxWidget::drawChannelData(ViewType plane,
//                                       std::vector<OmTilePtr> drawables)
// {
//     ChannelDataWrapper cdw(1);
//
//     if(!Om3dPreferences::get2DViewPaneIn3D() || !cdw.IsChannelValid()){
//         return;
//     }
//
//     OmChannel& channel = cdw.GetChannel();
//     Vector3f resolution = channel.Coords().Resolution();
//     Vector3i extents = channel.Coords().DataDimensions();
//
//     glColor3fv(OMGL_WHITE);
//     glEnable(GL_TEXTURE_2D);
//     glDepthMask(GL_TRUE);
//
//     Vector2f dataMin,dataMax,spaceMin,spaceMax;
//
//     for(OmTilePtr d : drawables)
//     {
//         OmTilePtr d = *it;
//
//         const int level = d->GetTileCoord().mipLevel();
//         const Vector3f tileLength =
// resolution*128.0*om::math::pow2int(level);
//
//         const DataCoord thisCoord = d->GetTileCoord().getDataCoord();
//         //debug ("FIXME", "thisCoord.(x,y,z): (%f,%f,%f)\n",
// DEBUGV3(thisCoord));
//         const NormCoord normCoord =
//             channel.Coords().DataToNormCoord(d->GetTileCoord().getDataCoord());
//
//         //debug ("FIXME", "normCoord.(x,y,z): (%f,%f,%f)\n",
// DEBUGV3(normCoord));
//         glBindTexture(GL_TEXTURE_2D, d->GetTexture().GetTextureID());
//         glBegin(GL_QUADS);
//
//         if (plane == om::common::ViewType::XY_VIEW) {
//             if (getTextureMax(thisCoord, plane, dataMax, spaceMax)){
//                 spaceMax.x = thisCoord.x + tileLength.x;
//                 spaceMax.y = thisCoord.y + tileLength.y;
//                 dataMax.x = 1.0;
//                 dataMax.y = 1.0;
//             }
//             if (getTextureMin(thisCoord, plane, dataMin, spaceMin)){
//                 spaceMin.x = thisCoord.x;
//                 spaceMin.y = thisCoord.y;
//                 dataMin.x = 0.0;
//                 dataMin.y = 0.0;
//             }
//             glTexCoord2f(dataMin.x, dataMin.y);  /* lower left corner of
// image */
//             glVertex3f(spaceMin.x, spaceMin.y,thisCoord.z);
//
//             glTexCoord2f(dataMax.x, dataMin.y);  /* lower right corner of
// image */
//             glVertex3f(spaceMax.x, spaceMin.y,thisCoord.z);
//
//             glTexCoord2f(dataMax.x, dataMax.y);  /* upper right corner of
// image */
//             glVertex3f(spaceMax.x, spaceMax.y, thisCoord.z);
//
//             glTexCoord2f(dataMin.x, dataMax.y);  /* upper left corner of
// image */
//             glVertex3f(spaceMin.x, spaceMax.y, thisCoord.z);
//             glEnd();
//         } else if (plane == om::common::ViewType::XZ_VIEW) {
//             if (getTextureMax(thisCoord, plane, dataMax, spaceMax)){
//                 spaceMax.x = thisCoord.x + tileLength.x;
//                 spaceMax.y = thisCoord.z + tileLength.z;
//                 dataMax.x = 1.0;
//                 dataMax.y = 1.0;
//             }
//             if (getTextureMin(thisCoord, plane, dataMin, spaceMin)){
//                 spaceMin.x = thisCoord.x;
//                 spaceMin.y = thisCoord.z;
//                 dataMin.x = 0.0;
//                 dataMin.y = 0.0;
//             }
//             glTexCoord2f(dataMin.x, dataMin.y);  /* lower left corner of
// image */
//             glVertex3f(spaceMin.x, thisCoord.y,spaceMin.y);
//
//             glTexCoord2f(dataMax.x, dataMin.y);  /* lower right corner of
// image */
//             glVertex3f(spaceMax.x, thisCoord.y,spaceMin.y);
//
//             glTexCoord2f(dataMax.x, dataMax.y);  /* upper right corner of
// image */
//             glVertex3f(spaceMax.x, thisCoord.y,spaceMax.y);
//
//             glTexCoord2f(dataMin.x, dataMax.y);  /* upper left corner of
// image */
//             glVertex3f(spaceMin.x, thisCoord.y,spaceMax.y);
//             glEnd();
//         } else if (plane == om::common::ViewType::ZY_VIEW) {
//             if (getTextureMax(thisCoord, plane, dataMax, spaceMax)){
//                 spaceMax.x = thisCoord.z + tileLength.z;
//                 spaceMax.y = thisCoord.y + tileLength.y;
//                 dataMax.x = 1.0;
//                 dataMax.y = 1.0;
//             }
//             if (getTextureMin(thisCoord, plane, dataMin, spaceMin)){
//                 spaceMin.x = thisCoord.z;
//                 spaceMin.y = thisCoord.y;
//                 dataMin.x = 0.0;
//                 dataMin.y = 0.0;
//             }
//
//             //debug ("chandata", "dataMin.(x,y): (%f,%f)\n",
// dataMin.x,dataMin.y);
//             //debug ("chandata", "dataMax.(x,y): (%f,%f)\n",
// dataMax.x,dataMax.y);
//             //debug ("chandata", "spaceMin.(x,y): (%f,%f)\n",
// spaceMin.x,spaceMin.y);
//             //debug ("chandata", "spaceMax.(x,y): (%f,%f)\n",
// spaceMax.x,spaceMax.y);
//             glTexCoord2f(dataMin.x, dataMin.y);  /* lower left corner of
// image */
//             glVertex3f(thisCoord.x,spaceMin.x,spaceMin.y);
//
//             glTexCoord2f(dataMax.x, dataMin.y);  /* lower right corner of
// image */
//             glVertex3f(thisCoord.x,spaceMax.x ,spaceMin.y);
//
//             glTexCoord2f(dataMax.x, dataMax.y);  /* upper right corner of
// image */
//             glVertex3f(thisCoord.x,spaceMax.x,spaceMax.y);
//
//             glTexCoord2f(dataMin.x, dataMax.y);  /* upper left corner of
// image */
//             glVertex3f(thisCoord.x,spaceMin.x,spaceMax.y);
//             glEnd();
//         }
//     }
//     //glEnable(GL_DEPTH_TEST);
//     glDepthMask(GL_FALSE);
//     glDisable(GL_BLEND);
//     glDisable(GL_TEXTURE_2D);
// }

// bool ViewBoxWidget::getTextureMax(Vector3f coord,
//                                     om::common::ViewType plane,
//                                     Vector2f & dataMax,
//                                     Vector2f & spaceMax)
// {
//     OmChannel& channel = OmProject::Volumes().Channels().GetChannel( 1);
//     Vector3f resolution = channel.Coords().GetDataResolution();
//     Vector3f tileLength = resolution*128.0;
//     Vector2f maxScreen = vgs_.View2dState()->GetViewSliceMax(plane);
//     DataCoord maxDataExt = channel.Coords().DataExtent().getMax();
//     NormCoord maxNorm = channel.Coords().DataToNormCoord(maxDataExt);
//     DataCoord maxData= channel.Coords().NormToData(maxNorm);
//     DataCoord maxLimit = maxData.compareMinimum(coord+tileLength);
//     bool result;
//
//     switch (plane) {
//     case om::common::ViewType::XY_VIEW:
//         if (maxLimit.x>maxScreen.x) spaceMax.x=maxScreen.x; else
// spaceMax.x=maxLimit.x;
//         if (maxLimit.y>maxScreen.y) spaceMax.y=maxScreen.y; else
// spaceMax.y=maxLimit.y;
//         dataMax.x = (spaceMax.x-coord.x)/tileLength.x;
//         dataMax.y = (spaceMax.y-coord.y)/tileLength.y;
//         result = ((coord.x + tileLength.x) < spaceMax.x);
//         result = result && ((coord.y + tileLength.y) < spaceMax.y);
//         break;
//     case om::common::ViewType::XZ_VIEW:
//         if (maxLimit.x>maxScreen.x) spaceMax.x=maxScreen.x; else
// spaceMax.x=maxLimit.x;
//         if (maxLimit.z>maxScreen.y) spaceMax.y=maxScreen.y; else
// spaceMax.y=maxLimit.z;
//         dataMax.x = (spaceMax.x-coord.x)/tileLength.x;
//         dataMax.y = (spaceMax.y-coord.z)/tileLength.z;
//         result = ((coord.x + tileLength.x) < spaceMax.x);
//         result = result && ((coord.z + tileLength.z) < spaceMax.y);
//         break;
//     case om::common::ViewType::ZY_VIEW:
//         if (maxLimit.z>maxScreen.y) spaceMax.y=maxScreen.y; else
// spaceMax.y=maxLimit.z;
//         if (maxLimit.y>maxScreen.x) spaceMax.x=maxScreen.x; else
// spaceMax.x=maxLimit.y;
//         dataMax.x = (spaceMax.x-coord.y)/tileLength.y;
//         dataMax.y = (spaceMax.y-coord.z)/tileLength.z;
//         result = ((coord.y + tileLength.y) < spaceMax.x);
//         result = result && ((coord.z + tileLength.z) < spaceMax.y);
//         break;
//     default:
//         assert(0);
//     }
//     return result;
// }

// bool
// ViewBoxWidget::getTextureMin(Vector3f coord,ViewType plane, Vector2f &
// dataMin, Vector2f & spaceMin)
// {
//
//     OmChannel& channel = OmProject::Volumes().Channels().GetChannel( 1);
//
//     Vector3f resolution = channel.Coords().GetDataResolution();
//     Vector3f tileLength = resolution*128.0;
//     Vector2f minScreen = vgs_.View2dState()->GetViewSliceMin(plane);
//     DataCoord minDataExt = channel.Coords().DataExtent().getMin();
//     NormCoord minNorm = channel.Coords().DataToNormCoord(minDataExt);
//     DataCoord minData= channel.Coords().NormToData(minNorm);
//     DataCoord minLimit = minData.compareMaximum(coord);
//     bool result;
//
//     switch (plane) {
//     case om::common::ViewType::XY_VIEW:
//         if (minLimit.x<minScreen.x) spaceMin.x=minScreen.x; else
// spaceMin.x=minLimit.x;
//         if (minLimit.y<minScreen.y) spaceMin.y=minScreen.y; else
// spaceMin.y=minLimit.y;
//         dataMin.x = (spaceMin.x - coord.x)/tileLength.x;
//         dataMin.y = (spaceMin.y - coord.y)/tileLength.y;
//         result = (coord.x > spaceMin.x);
//         result = result && (coord.y > spaceMin.y);
//         break;
//     case om::common::ViewType::XZ_VIEW:
//         if (minLimit.x<minScreen.x) spaceMin.x=minScreen.x; else
// spaceMin.x=minLimit.x;
//         if (minLimit.z<minScreen.y) spaceMin.y=minScreen.y; else
// spaceMin.y=minLimit.z;
//         dataMin.x = (spaceMin.x - coord.x)/tileLength.x;
//         dataMin.y = (spaceMin.y - coord.z)/tileLength.z;
//         result = (coord.x > spaceMin.x);
//         result = result && (coord.z > spaceMin.y);
//         break;
//     case om::common::ViewType::ZY_VIEW:
//         if (minLimit.z<minScreen.y) spaceMin.y=minScreen.y; else
// spaceMin.y=minLimit.z;
//         if (minLimit.y<minScreen.x) spaceMin.x=minScreen.x; else
// spaceMin.x=minLimit.y;
//         dataMin.x = (spaceMin.x - coord.y)/tileLength.y;
//         dataMin.y = (spaceMin.y - coord.z)/tileLength.z;
//         result = (coord.z > spaceMin.y);
//         result = result && (coord.y > spaceMin.x);
//         break;
//     default:
//         assert(0);
//     }
//     return result;
// }
