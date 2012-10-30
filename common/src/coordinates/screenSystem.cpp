#include "common/common.h"
#include "coordinates/screenSystem.h"
#include "vmmlib/vmmlib.h"
using namespace vmml;
using namespace om::common;

namespace om {
namespace coords {

screenSystem::screenSystem(common::viewType viewType)
    : viewType_(viewType)
{ }

void screenSystem::UpdateTransformationMatrices()
{
    // init
    screenToGlobalMat_ = Matrix4f::ZERO;
    globalToScreenMat_ = Matrix4f::ZERO;

    const Vector3f pan = makeViewTypeVector3(totalViewport_.width/2.0f,
                                                totalViewport_.height/2.0f,
                                                0.0f);

    // g to s scaling and translation
    globalToScreenMat_.setTranslation(makeViewTypeVector3(pan / zoomScale_ - location_));
    screenToGlobalMat_.setTranslation(location_ * zoomScale_ - pan);

    globalToScreenMat_.m33 = 1 / zoomScale_;
    screenToGlobalMat_.m33 = zoomScale_;

    globalToScreenMat_.m23 = 0; // Makes sure the z value in screen coords is 0

    // selection matrices by viewType with twisty s to g translations and scaling
    switch(viewType_)
    {
        case XY_VIEW:
            globalToScreenMat_.m00 = 1;
            globalToScreenMat_.m11 = 1;
            screenToGlobalMat_.m00 = 1;
            screenToGlobalMat_.m11 = 1;
            break;

        case XZ_VIEW:
            globalToScreenMat_.m00 = 1;
            globalToScreenMat_.m12 = 1;
            screenToGlobalMat_.m00 = 1;
            screenToGlobalMat_.m21 = 1;
            break;

        case ZY_VIEW:
            globalToScreenMat_.m02 = 1;
            globalToScreenMat_.m11 = 1;
            screenToGlobalMat_.m20 = 1;
            screenToGlobalMat_.m11 = 1;
            break;
    }
}

} // namespace coords
} // namespace om