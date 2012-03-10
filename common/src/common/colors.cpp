#include "common/colors.h"

namespace om {
namespace common {
        
std::ostream& operator<<(std::ostream &out, const color& c)
{
    out << "[r" << (int)c.red
        << ",g" << (int)c.green
        << ",b" << (int)c.blue
        << "]";
    return out;
}

std::ostream& operator<<(std::ostream &out, const colorARGB& c)
{
    out << "[a" << (int)c.alpha
        << ",r" << (int)c.red
        << ",g" << (int)c.green
        << ",b" << (int)c.blue
        << "]";
    return out;
}

bool operator<(const color& a, const color& b)
{
    if(a.red != b.red){
        return a.red < b.red;
    }

    if(a.green != b.green){
        return a.green < b.green;
    }

    return a.blue < b.blue;
}

bool operator==(const color& a, const color& b)
{
    return a.red == b.red &&
        a.green == b.green &&
        a.blue == b.blue;
}

} // namespace common
} // namespace om