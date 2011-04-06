#include "common/omColors.h"

std::ostream& operator<<(std::ostream &out, const OmColor& c)
{
    out << "[r" << (int)c.red
        << ",g" << (int)c.green
        << ",b" << (int)c.blue
        << "]";
    return out;
}

std::ostream& operator<<(std::ostream &out, const OmColorARGB& c)
{
    out << "[a" << (int)c.alpha
        << ",r" << (int)c.red
        << ",g" << (int)c.green
        << ",b" << (int)c.blue
        << "]";
    return out;
}

bool operator<(const OmColor& a, const OmColor& b)
{
    if(a.red != b.red){
        return a.red < b.red;
    }

    if(a.green != b.green){
        return a.green < b.green;
    }

    return a.blue < b.blue;
}
