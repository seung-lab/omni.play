#pragma once

#include "common/om.hpp"
#include "common/omCommon.h"
#include "events/omEvents.h"

/**
 * Global viewing location across all volumes
 **/

class OmViewGroupView2dState {
 private:
  struct ViewPlanInfo {
    float depth;
    Vector2f min;
    Vector2f max;
  };

  ViewPlanInfo xy_;
  ViewPlanInfo xz_;
  ViewPlanInfo yz_;

  inline ViewPlanInfo& getPlane(const ViewType plane) {
    switch (plane) {
      case XY_VIEW:
        return xy_;
      case XZ_VIEW:
        return xz_;
      case ZY_VIEW:
        return yz_;
      default:
        throw OmArgException("unknown plane");
    }
  }

  inline const ViewPlanInfo& getPlane(const ViewType plane) const {
    switch (plane) {
      case XY_VIEW:
        return xy_;
      case XZ_VIEW:
        return xz_;
      case ZY_VIEW:
        return yz_;
      default:
        throw OmArgException("unknown plane");
    }
  }

  // Get() methods could be called from threads while main GUI thread is calling
  // Set(...) methods
  zi::spinlock lock_;

  bool wasInitialized_;

 public:
  OmViewGroupView2dState() : wasInitialized_(false) {}

  ~OmViewGroupView2dState() {}

  void SetInitialized() { wasInitialized_ = true; }

  bool GetInitialized() const { return wasInitialized_; }

  inline om::globalCoord GetScaledSliceDepth() const {
    zi::guard g(lock_);
    return om::globalCoord(yz_.depth, xz_.depth, xy_.depth);
  }

  inline float GetScaledSliceDepth(const ViewType plane) const {
    zi::guard g(lock_);
    return getPlane(plane).depth;
  }

  inline void SetScaledSliceDepth(const om::globalCoord& depths) {
    {
      zi::guard g(lock_);
      yz_.depth = depths.x;
      xz_.depth = depths.y;
      xy_.depth = depths.z;
    }
    OmEvents::ViewBoxChanged();
  }

  inline void SetScaledSliceDepth(const ViewType plane, const float depth) {
    {
      zi::guard g(lock_);
      getPlane(plane).depth = depth;
    }
    OmEvents::ViewBoxChanged();
  }

  // minimum coordiante of view slice
  inline void SetViewSliceMin(const ViewType plane, const Vector2f vec) {
    zi::guard g(lock_);
    getPlane(plane).min = vec;
  }

  inline Vector2f GetViewSliceMin(ViewType plane) const {
    const Vector2f& pts = getPlane(plane).min;
    {
      zi::guard g(lock_);
      return Vector2f(pts.x, pts.y);
    }
  }

  // maximum coordiante of view slice
  inline void SetViewSliceMax(const ViewType plane, const Vector2f vec) {
    zi::guard g(lock_);
    getPlane(plane).max = vec;
  }

  inline Vector2f GetViewSliceMax(const ViewType plane) const {
    const Vector2f& pts = getPlane(plane).max;
    {
      zi::guard g(lock_);
      return Vector2f(pts.x, pts.y);
    }
  }
};
