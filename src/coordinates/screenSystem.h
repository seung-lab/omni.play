#pragma once

#include "vmmlib/vmmlib.h"
using namespace vmml;

namespace om {
namespace coords {

class globalCoord;

class screenSystem
{
private:
    common::viewType viewType_;
    Vector4i totalViewport_; //lower left x, lower left y, width, height
    
    Matrix4f screenToGlobalMat_;
    Matrix4f globalToScreenMat_;
    
    double zoomScale_;
    globalCoord location_;
    
    template <typename T>
    inline Vector3<T> makeViewTypeVector3(const T& x, const T& y, const T& z) const 
    {
        switch(viewType_){
            case common::XY_VIEW:
                return Vector3<T>(x, y, z);
            case common::XZ_VIEW:
                return Vector3<T>(x, z, y);
            case common::ZY_VIEW:
                return Vector3<T>(z, y, x);
            default:
                throw common::argException("invalid viewType");
        }
    }
    
    template <typename T>
    inline Vector3<T> makeViewTypeVector3(const Vector3<T>& vec) const 
    {
        return makeViewTypeVector3(vec.x, vec.y, vec.z);
    }
    
public:
    screenSystem(common::viewType viewType);
    
    // Update the Transformation Matricies based on changes to scale, location or viewport
    //
    // transformation from global to screen should be equivalent to these linear equations:
    // xs = (xg + w/2 - lx) * scale
    // ys = (yg + h/2 - ly) * scale
    //
    // In the case of the different views, different values will be chosen as X and Y.
    // Here are the selection matrices:
    //
    //    XY      XZ      ZY
    // [1 0 0] [1 0 0] [0 0 1]
    // [0 1 0] [0 0 1] [0 1 0]
    // [0 0 0] [0 0 0] [0 0 0]
    //
    // transformation from screen to global should be equivalent to these linear equations:
    // xg = xs / scale - (w/2 - lx)
    // yg = ys / scale - (h/2 - ly)
    // zg = lz
    //
    // the selection matrices should be transposes of the earlier ones and the transforms need 
    // to be shuffled around too.
    void UpdateTransformationMatrices();
    
    inline const Matrix4f& ScreenToGlobalMat() const {
        return screenToGlobalMat_;
    }
    
    inline const Matrix4f& GlobalToScreenMat() const {
        return globalToScreenMat_;
    }
    
    // viewport
    inline const Vector4i& getTotalViewport() const {
        return totalViewport_;
    }
    
    inline void setTotalViewport(const int width, const int height) {
        totalViewport_ = Vector4i(0, 0, width, height);
    }
    
    inline void setZoomScale(const double scale) {
        zoomScale_ = scale;
    }
    
    inline void setLocation(const globalCoord location) {
        location_ = location;
    }
};

} // namespace coords
} // namespace om