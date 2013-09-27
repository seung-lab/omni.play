#pragma once

#include "common/macro.hpp"
#include "coordinates/coordinates.h"

namespace om {
namespace coords {

class Global;

class ScreenSystem {
 private:
  template <typename T>
  inline Vector3<T> makeViewTypeVector3(const T& x, const T& y,
                                        const T& z) const {
    switch (viewType_) {
      case common::ViewType::XY_VIEW:
        return Vector3<T>(x, y, z);
      case common::ViewType::XZ_VIEW:
        return Vector3<T>(x, z, y);
      case common::ViewType::ZY_VIEW:
        return Vector3<T>(z, y, x);
    }
    return Vector3<T>();
  }

  template <typename T>
  inline Vector3<T> makeViewTypeVector3(const Vector3<T>& vec) const {
    return makeViewTypeVector3(vec.x, vec.y, vec.z);
  }

 public:
  ScreenSystem(om::common::ViewType viewType);
  ScreenSystem(om::common::ViewType viewType, int width, int height,
               double scale = 1.0, Global location = Global(0));

  // Update the Transformation Matricies based on changes to scale, location or
  // viewport
  //
  // transformation from global to screen should be equivalent to these linear
  // equations:
  // xs = (xg - lx) * scale + w/2
  // ys = (yg - ly) * scale + h/2
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
  // xg = (xs - w/2) / scale + lx
  // yg = (ys - h/2) / scale + ly
  // zg = lz
  //
  // the selection matrices should be transposes of the earlier ones and the
  // transforms need
  // to be shuffled around too.
  void UpdateTransformationMatrices();

 private:
  PROP_CONST_REF(om::common::ViewType, viewType);

  PROP_CONST_REF(Matrix4f, screenToGlobalMat);
  PROP_CONST_REF(Matrix4f, globalToScreenMat);

  PROP_REF_SET(Vector4i, totalViewport);
  PROP_REF_SET(double, zoomScale);
  PROP_REF_SET(Global, location);
};

}  // namespace coords
}  // namespace om
