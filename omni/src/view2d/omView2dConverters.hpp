#ifndef OM_VIEW2D_CONVERTERS_HPP
#define OM_VIEW2D_CONVERTERS_HPP

#include "common/omCommon.h"

class OmView2dConverters {
public:
    template <typename T>
    inline static Vector3<T> MakeViewTypeVector3(const T& x, const T& y,
                                                 const T& z,
                                                 const ViewType viewType)
    {
        switch(viewType){
        case XY_VIEW:
            return Vector3<T>(x, y, z);
        case XZ_VIEW:
            return Vector3<T>(x, z, y);
        case YZ_VIEW:
            return Vector3<T>(z, y, x);
        default:
            throw OmArgException("invalid viewType");
        }
    }

    template <typename T>
    inline static Vector3<T> MakeViewTypeVector3(const Vector3<T>& vec,
                                                 const ViewType viewType)
    {
        switch(viewType){
        case XY_VIEW:
            return vec;
        case XZ_VIEW:
            return Vector3<T>(vec.x, vec.z, vec.y);
        case YZ_VIEW:
            return Vector3<T>(vec.z, vec.y, vec.x);
        default:
            throw OmArgException("invalid viewType");
        }
    }

    template <typename T>
    inline static T GetViewTypeDepth(const Vector3<T>& vec,
                                     const ViewType viewType)
    {
        switch(viewType){
        case XY_VIEW:
            return vec.z;
        case XZ_VIEW:
            return vec.y;
        case YZ_VIEW:
            return vec.x;
        default:
            throw OmArgException("invalid viewType");
        }
    }

    template <typename T>
    inline static void SetViewTypeDepth(Vector3<T>& vec, const T& val,
                                        const ViewType viewType)
    {
        switch(viewType){
        case XY_VIEW:
            vec.z = val;
            break;
        case XZ_VIEW:
            vec.y = val;
            break;
        case YZ_VIEW:
            vec.x = val;
            break;
        default:
            throw OmArgException("invalid viewType");
        }
    }

    template <typename T>
    inline static Vector3<T> ScaleViewType(const Vector2<T>& vec,
                                           const Vector3<T>& scale,
                                           const ViewType viewType)
    {
        return ScaleViewType(vec.x, vec.y, scale, viewType);
    }

    template <typename T>
    inline static Vector3<T> ScaleViewType(const T& x, const T& y,
                                           const Vector3<T>& scale,
                                           const ViewType viewType)
    {
        switch(viewType){
        case XY_VIEW:
            return Vector3<T>(x / scale.x,
                              y / scale.y,
                              0);
        case XZ_VIEW:
            return Vector3<T>(x / scale.x,
                              0,
                              y / scale.z);
        case YZ_VIEW:
            return Vector3<T>(0,
                              y / scale.y,
                              x / scale.z);
        default:
            throw OmArgException("invalid viewType");
        }
    }

    template <typename T>
    inline static Vector2<T> Get2PtsInPlane(const Vector3<T>& vec,
                                            const ViewType viewType)
    {
        switch(viewType) {
        case XY_VIEW:
            return Vector2<T>(vec.x, vec.y);
        case XZ_VIEW:
            return Vector2<T>(vec.x, vec.z);
        case YZ_VIEW:
            return Vector2<T>(vec.z, vec.y);
        default:
            throw OmArgException("unknown viewType");
        }
    }

private:
    template <typename T>
    inline static void incLeftRightPanDirection(Vector3<T>& vec, const T val,
                                                const ViewType viewType)
    {
        switch(viewType) {
        case XY_VIEW:
            vec.x += val;
            break;
        case XZ_VIEW:
            vec.x += val;
            break;
        case YZ_VIEW:
            vec.z += val;
            break;
        default:
            throw OmArgException("unknown viewType");
        }
    }

    template <typename T>
    inline static void incUpDownPanDirection(Vector3<T>& vec, const T val,
                                             const ViewType viewType)
    {
        switch(viewType) {
        case XY_VIEW:
            vec.y += val;
            break;
        case XZ_VIEW:
            vec.z += val;
            break;
        case YZ_VIEW:
            vec.y += val;
            break;
        default:
            throw OmArgException("unknown viewType");
        }
    }

public:
    template <typename T>
    inline static void ShiftPanDirection(Vector3<T>& vec, const T val,
                                         const om::Direction dir, const ViewType viewType)
    {
        switch(dir){
        case om::LEFT:
            incLeftRightPanDirection(vec, val, viewType);
            break;
        case om::RIGHT:
            incLeftRightPanDirection(vec, -val, viewType);
            break;
        case om::UP:
            incUpDownPanDirection(vec, val, viewType);
            break;
        case om::DOWN:
            incUpDownPanDirection(vec, -val, viewType);
            break;
        default:
            throw OmArgException("unknown direction");
        }
    }

    template <typename T>
    inline static Vector3<T> IncVectorWith2Pts(const Vector3<T>& vec, const T x, const T y,
                                               const ViewType viewType)
    {
        switch(viewType){
        case XY_VIEW:
            return Vector3<T>(vec.x + x, vec.y + y, vec.z);
        case XZ_VIEW:
            return Vector3<T>(vec.x + x, vec.y, vec.z + y);
        case YZ_VIEW:
            return Vector3<T>(vec.x, vec.y + y, vec.z + x);
        default:
            throw OmArgException("invalid viewType");
        }
    }
};

#endif
