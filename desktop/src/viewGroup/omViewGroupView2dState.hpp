#pragma once

#include "common/common.h"
#include "events/omEvents.h"

/**
 * Global viewing location across all volumes
 **/

class OmViewGroupView2dState {
private:
    struct ViewPlanInfo
    {
        float depth;
        Vector2f min;
        Vector2f max;
    };

    ViewPlanInfo xy_;
    ViewPlanInfo xz_;
    ViewPlanInfo yz_;

    inline ViewPlanInfo& getPlane(const om::common::ViewType plane)
    {
        switch (plane) {
        case om::common::XY_VIEW:
            return xy_;
        case om::common::XZ_VIEW:
            return xz_;
        case om::common::ZY_VIEW:
            return yz_;
        default:
            throw om::ArgException("unknown plane");
        }
    }

    inline const ViewPlanInfo& getPlane(const om::common::ViewType plane) const
    {
        switch (plane) {
        case om::common::XY_VIEW:
            return xy_;
        case om::common::XZ_VIEW:
            return xz_;
        case om::common::ZY_VIEW:
            return yz_;
        default:
            throw om::ArgException("unknown plane");
        }
    }

    // Get() methods could be called from threads while main GUI thread is calling Set(...) methods
    zi::spinlock lock_;

    bool wasInitialized_;

public:
    OmViewGroupView2dState()
        : wasInitialized_(false)
    {}

    ~OmViewGroupView2dState()
    {}

    void SetInitialized(){
        wasInitialized_ = true;
    }

    bool GetInitialized() const {
        return wasInitialized_;
    }

    inline om::coords::Global GetScaledSliceDepth() const
    {
        zi::guard g(lock_);
        return om::coords::Global(yz_.depth, xz_.depth, xy_.depth);
    }

    inline float GetScaledSliceDepth(const om::common::ViewType plane) const
    {
        zi::guard g(lock_);
        return getPlane(plane).depth;
    }

    inline void SetScaledSliceDepth(const om::coords::Global& depths)
    {
        {
            zi::guard g(lock_);
            yz_.depth = depths.x;
            xz_.depth = depths.y;
            xy_.depth = depths.z;
        }
        OmEvents::ViewBoxChanged();
    }

    inline void SetScaledSliceDepth(const om::common::ViewType plane, const float depth)
    {
        {
            zi::guard g(lock_);
            getPlane(plane).depth = depth;
        }
        OmEvents::ViewBoxChanged();
    }

    // minimum coordiante of view slice
    inline void SetViewSliceMin(const om::common::ViewType plane, const Vector2f vec)
    {
        zi::guard g(lock_);
        getPlane(plane).min = vec;
    }

    inline Vector2f GetViewSliceMin(om::common::ViewType plane) const
    {
        const Vector2f& pts = getPlane(plane).min;
        {
            zi::guard g(lock_);
            return Vector2f(pts.x, pts.y);
        }
    }

// maximum coordiante of view slice
    inline void SetViewSliceMax(const om::common::ViewType plane, const Vector2f vec)
    {
        zi::guard g(lock_);
        getPlane(plane).max = vec;
    }

    inline Vector2f GetViewSliceMax(const om::common::ViewType plane) const
    {
        const Vector2f& pts = getPlane(plane).max;
        {
            zi::guard g(lock_);
            return Vector2f(pts.x, pts.y);
        }
    }
};

