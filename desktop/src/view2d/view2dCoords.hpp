#pragma once

#include "vmmlib/vmmlib.h"
#include "viewGroup/omZoomLevel.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "view2d/omView2dConverters.hpp"
#include <QSize>

using namespace vmml;

namespace om {

class view2dCoords {
 private:
  OmViewGroupState* vgs_;
  OmZoomLevel* zoomLevel_;
  om::common::ViewType viewType_;
  Vector4i totalViewport_;  // lower left x, lower left y, width, height

  Matrix4f screenToGlobalMat_;
  Matrix4f globalToScreenMat_;

  template <typename T>
  inline Vector3<T> makeViewTypeVector3(const T& x, const T& y,
                                        const T& z) const {
    return OmView2dConverters::MakeViewTypeVector3(x, y, z, viewType_);
  }

  template <typename T>
  inline Vector3<T> makeViewTypeVector3(const Vector3<T>& vec) const {
    return OmView2dConverters::MakeViewTypeVector3(vec, viewType_);
  }

 public:
  view2dCoords(OmViewGroupState* vgs, om::common::ViewType viewType)
      : vgs_(vgs), zoomLevel_(vgs->ZoomLevel()), viewType_(viewType) {}

  // Update the Transformation Matricies based on changes to scale, location or
  // viewport
  //
  // transformation from global to screen should be equivalent to these linear
  // equations:
  // xs = (xg + w/2 - lx) * scale
  // ys = (yg + h/2 - ly) * scale
  //
  // In the case of the different views, different values will be chosen as X
  // and Y.
  // Here are the selection matrices:
  //
  //    XY      XZ      ZY
  // [1 0 0] [1 0 0] [0 0 1]
  // [0 1 0] [0 0 1] [0 1 0]
  // [0 0 0] [0 0 0] [0 0 0]
  //
  // transformation from screen to global should be equivalent to these linear
  // equations:
  // xg = xs / scale - (w/2 - lx)
  // yg = ys / scale - (h/2 - ly)
  // zg = lz
  //
  // the selection matrices should be transposes of the earlier ones and the
  // transforms need
  // to be shuffled around too.
  void UpdateTransformationMatrices() {
    // init
    screenToGlobalMat_ = Matrix4f::ZERO;
    globalToScreenMat_ = Matrix4f::ZERO;

    float scale = zoomLevel_->GetZoomScale();

    const Vector3f pan = makeViewTypeVector3(
        totalViewport_.width / 2.0f, totalViewport_.height / 2.0f, 0.0f);

    om::coords::Global location = vgs_->View2dState()->GetScaledSliceDepth();

    // g to s scaling and translation
    globalToScreenMat_.setTranslation(
        makeViewTypeVector3(pan / scale - location));
    screenToGlobalMat_.setTranslation(location * scale - pan);

    globalToScreenMat_.m33 = 1 / scale;
    screenToGlobalMat_.m33 = scale;

    globalToScreenMat_.m23 = 0;  // Makes sure the z value in screen coords is 0

    // selection matrices by viewType with twisty s to g translations and
    // scaling
    switch (viewType_) {
      case om::common::XY_VIEW:
        globalToScreenMat_.m00 = 1;
        globalToScreenMat_.m11 = 1;
        screenToGlobalMat_.m00 = 1;
        screenToGlobalMat_.m11 = 1;
        break;

      case om::common::XZ_VIEW:
        globalToScreenMat_.m00 = 1;
        globalToScreenMat_.m12 = 1;
        screenToGlobalMat_.m00 = 1;
        screenToGlobalMat_.m21 = 1;
        break;

      case om::common::ZY_VIEW:
        globalToScreenMat_.m02 = 1;
        globalToScreenMat_.m11 = 1;
        screenToGlobalMat_.m20 = 1;
        screenToGlobalMat_.m11 = 1;
        break;
    }
  }

  inline const Matrix4f& ScreenToGlobalMat() const {
    return screenToGlobalMat_;
  }

  inline const Matrix4f& GlobalToScreenMat() const {
    return globalToScreenMat_;
  }

  // viewport
  inline const Vector4i& getTotalViewport() const { return totalViewport_; }

  void setTotalViewport(const QSize& size) {
    totalViewport_ = Vector4i(0, 0, size.width(), size.height());
    UpdateTransformationMatrices();
  }
  void setTotalViewport(const int width, const int height) {
    totalViewport_ = Vector4i(0, 0, width, height);
    UpdateTransformationMatrices();
  }
};
}  // namespace om