#include "common/common.h"

namespace om {
namespace coords {

Global Screen::ToGlobal() const
{
    const vmml::Vector4f screen(x,y,0,1);
    vmml::Vector3f global = system_->screenToGlobalMat() * screen;
    return global;
}

} // namespace coords
} // namespace om