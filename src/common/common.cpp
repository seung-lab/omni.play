#include "common/common.h"
#include "string.hpp"
#include "zi/utility.h"

namespace om {
namespace common {

viewType Convert(server::viewType::type type) {
    switch(type) {
    case server::viewType::XY_VIEW: return XY_VIEW;
    case server::viewType::XZ_VIEW: return XZ_VIEW;
    case server::viewType::ZY_VIEW: return ZY_VIEW;
    }
    throw argException("Bad viewType.");
}

server::viewType::type Convert(viewType type) {
    switch(type) {
    case XY_VIEW: return server::viewType::XY_VIEW;
    case XZ_VIEW: return server::viewType::XZ_VIEW;
    case ZY_VIEW: return server::viewType::ZY_VIEW;
    }
    throw argException("Bad viewType.");
}

std::ostream& operator<<(std::ostream &out, const segIdSet& in)
{
    const std::string joined = om::string::join(in);

    out << "[" << joined << "]";
    return out;
}

std::ostream& operator<<(std::ostream &out, const cacheGroup& c)
{
    if(MESH_CACHE == c){
        out << "MESH_CACHE";
    } else {
        out <<"TILE_CACHE";
    }
    return out;
}

std::ostream& operator<<(std::ostream &out, const viewType& vt)
{
    switch(vt){
    case XY_VIEW:
        out << "XY_VIEW";
        break;
    case XZ_VIEW:
        out << "XZ_VIEW";
        break;
    case ZY_VIEW:
        out << "ZY_VIEW";
        break;
    default:
        throw argException("unknown viewtype");
    }

    return out;
}

} // namespace common
} // namespace om
