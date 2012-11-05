#include "common/common.h"
#include "string.hpp"
#include "zi/utility.h"

namespace om {
namespace common {

std::ostream& operator<<(std::ostream &out, const SegIDSet& in)
{
    const std::string joined = om::string::join(in);

    out << "[" << joined << "]";
    return out;
}

std::ostream& operator<<(std::ostream &out, const ViewType& vt)
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
        throw ArgException("unknown viewtype");
    }

    return out;
}

} // namespace common
} // namespace om
