#ifndef OM_VIEW_GROUP_VIEW2D_STATE_HPP
#define OM_VIEW_GROUP_VIEW2D_STATE_HPP

#include "common/om.hpp"
#include "common/omCommon.h"
#include "events/omEvents.h"

/**
 * Global viewing location across all volumes
 **/

class OmViewGroupView2dState {
private:
    struct ViewPlanInfo
    {
        float depth;
        om::point2df min;
        om::point2df max;
    };

    ViewPlanInfo xy_;
    ViewPlanInfo xz_;
    ViewPlanInfo yz_;

    inline ViewPlanInfo& getPlane(const ViewType plane)
    {
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

    inline const ViewPlanInfo& getPlane(const ViewType plane) const
    {
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

    zi::spinlock lock_;

public:
    OmViewGroupView2dState()
    {}

    ~OmViewGroupView2dState()
    {}

    inline Vector3f GetScaledSliceDepth() const
    {
        zi::guard g(lock_);
        return Vector3f(yz_.depth, xz_.depth, xy_.depth);
    }

    inline float GetScaledSliceDepth(const ViewType plane) const
    {
        zi::guard g(lock_);
        return getPlane(plane).depth;
    }

    inline void SetScaledSliceDepth(const Vector3f& depths)
    {
        {
            zi::guard g(lock_);
            yz_.depth = depths.x;
            xz_.depth = depths.y;
            xy_.depth = depths.z;
        }
        OmEvents::ViewBoxChanged();
    }

    inline void SetScaledSliceDepth(const ViewType plane, const float depth)
    {
        {
            zi::guard g(lock_);
            getPlane(plane).depth = depth;
        }
        OmEvents::ViewBoxChanged();
    }

// minimum coordiante of view slice
    inline void SetViewSliceMin(const ViewType plane, const float x, const float y)
    {
        const om::point2df pts = { x, y };
        {
            zi::guard g(lock_);
            getPlane(plane).min = pts;
        }
    }

    inline Vector2f GetViewSliceMin(ViewType plane) const
    {
        const om::point2df& pts = getPlane(plane).min;
        {
            zi::guard g(lock_);
            return Vector2f(pts.x, pts.y);
        }
    }

// maximum coordiante of view slice
    inline void SetViewSliceMax(const ViewType plane, const float x, const float y)
    {
        const om::point2df pts = { x, y };
        {
            zi::guard g(lock_);
            getPlane(plane).max = pts;
        }
    }

    inline Vector2f GetViewSliceMax(const ViewType plane) const
    {
        const om::point2df& pts = getPlane(plane).max;
        {
            zi::guard g(lock_);
            return Vector2f(pts.x, pts.y);
        }
    }
};

#endif
