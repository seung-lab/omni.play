#include "common/common.h"

namespace om {
namespace coords {

Global screen::toGlobal() const
{
    const vmml::Vector4f screen(x,y,0,1);
    vmml::Vector3f global = system_->ScreenToGlobalMat() * screen;
    return global;
}

} // namespace coords
} // namespace om