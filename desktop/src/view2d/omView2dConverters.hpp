#pragma once
#include "precomp.h"

#include "common/common.h"

class OmView2dConverters {
 public:
  template <typename T>
  inline static Vector3<T> MakeViewTypeVector3(
      const T& x, const T& y, const T& z, const om::common::ViewType viewType) {
    switch (viewType) {
      case om::common::XY_VIEW:
        return Vector3<T>(x, y, z);
      case om::common::XZ_VIEW:
        return Vector3<T>(x, z, y);
      case om::common::ZY_VIEW:
        return Vector3<T>(z, y, x);
      default:
        throw om::ArgException("invalid viewType");
    }
  }

  template <typename T>
  inline static Vector3<T> MakeViewTypeVector3(
      const Vector3<T>& vec, const om::common::ViewType viewType) {
    switch (viewType) {
      case om::common::XY_VIEW:
        return vec;
      case om::common::XZ_VIEW:
        return Vector3<T>(vec.x, vec.z, vec.y);
      case om::common::ZY_VIEW:
        return Vector3<T>(vec.z, vec.y, vec.x);
      default:
        throw om::ArgException("invalid viewType");
    }
  }

  template <typename T>
  inline static T GetViewTypeDepth(const Vector3<T>& vec,
                                   const om::common::ViewType viewType) {
    switch (viewType) {
      case om::common::XY_VIEW:
        return vec.z;
      case om::common::XZ_VIEW:
        return vec.y;
      case om::common::ZY_VIEW:
        return vec.x;
      default:
        throw om::ArgException("invalid viewType");
    }
  }

  template <typename T>
  inline static void SetViewTypeDepth(Vector3<T>& vec, const T& val,
                                      const om::common::ViewType viewType) {
    switch (viewType) {
      case om::common::XY_VIEW:
        vec.z = val;
        break;
      case om::common::XZ_VIEW:
        vec.y = val;
        break;
      case om::common::ZY_VIEW:
        vec.x = val;
        break;
      default:
        throw om::ArgException("invalid viewType");
    }
  }

  template <typename T>
  inline static Vector3<T> ScaleViewType(const Vector2<T>& vec,
                                         const Vector3<T>& scale,
                                         const om::common::ViewType viewType) {
    return ScaleViewType(vec.x, vec.y, scale, viewType);
  }

  template <typename T>
  inline static Vector3<T> ScaleViewType(const T& x, const T& y,
                                         const Vector3<T>& scale,
                                         const om::common::ViewType viewType) {
    switch (viewType) {
      case om::common::XY_VIEW:
        return Vector3<T>(x / scale.x, y / scale.y, 0);
      case om::common::XZ_VIEW:
        return Vector3<T>(x / scale.x, 0, y / scale.z);
      case om::common::ZY_VIEW:
        return Vector3<T>(0, y / scale.y, x / scale.z);
      default:
        throw om::ArgException("invalid viewType");
    }
  }

  template <typename T>
  inline static Vector2<T> Get2PtsInPlane(const Vector3<T>& vec,
                                          const om::common::ViewType viewType) {
    switch (viewType) {
      case om::common::XY_VIEW:
        return Vector2<T>(vec.x, vec.y);
      case om::common::XZ_VIEW:
        return Vector2<T>(vec.x, vec.z);
      case om::common::ZY_VIEW:
        return Vector2<T>(vec.z, vec.y);
      default:
        throw om::ArgException("unknown viewType");
    }
  }

 private:
  template <typename T>
  inline static void incLeftRightPanDirection(
      Vector3<T>& vec, const T val, const om::common::ViewType viewType) {
    switch (viewType) {
      case om::common::XY_VIEW:
        vec.x += val;
        break;
      case om::common::XZ_VIEW:
        vec.x += val;
        break;
      case om::common::ZY_VIEW:
        vec.z += val;
        break;
      default:
        throw om::ArgException("unknown viewType");
    }
  }

  template <typename T>
  inline static void incUpDownPanDirection(
      Vector3<T>& vec, const T val, const om::common::ViewType viewType) {
    switch (viewType) {
      case om::common::XY_VIEW:
        vec.y += val;
        break;
      case om::common::XZ_VIEW:
        vec.z += val;
        break;
      case om::common::ZY_VIEW:
        vec.y += val;
        break;
      default:
        throw om::ArgException("unknown viewType");
    }
  }

 public:
  template <typename T>
  inline static void ShiftPanDirection(Vector3<T>& vec, const T val,
                                       const om::common::Direction dir,
                                       const om::common::ViewType viewType) {
    switch (dir) {
      case om::common::Direction::LEFT:
        incLeftRightPanDirection(vec, val, viewType);
        break;
      case om::common::Direction::RIGHT:
        incLeftRightPanDirection(vec, -val, viewType);
        break;
      case om::common::Direction::UP:
        incUpDownPanDirection(vec, val, viewType);
        break;
      case om::common::Direction::DOWN:
        incUpDownPanDirection(vec, -val, viewType);
        break;
      default:
        throw om::ArgException("unknown direction");
    }
  }

  template <typename T, typename ColT>
  inline static ColT IncVectorWith2Pts(const ColT& vec, const T x, const T y,
                                       const om::common::ViewType viewType) {
    switch (viewType) {
      case om::common::XY_VIEW:
        return ColT(vec.x + x, vec.y + y, vec.z);
      case om::common::XZ_VIEW:
        return ColT(vec.x + x, vec.y, vec.z + y);
      case om::common::ZY_VIEW:
        return ColT(vec.x, vec.y + y, vec.z + x);
      default:
        throw om::ArgException("invalid viewType");
    }
  }
};
