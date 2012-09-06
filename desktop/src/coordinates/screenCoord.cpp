#include "common/omCommon.h"
#include "view2d/omView2dState.hpp"

namespace om {

globalCoord screenCoord::toGlobalCoord() const
{
    const vmml::Vector4f screen(x,y,0,1);
    vmml::Vector3f global = state_->Coords().ScreenToGlobalMat() * screen;
    return global;
}

} // namespace om