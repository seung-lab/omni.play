#include "common/omCommon.h"
#include "common/omString.hpp"
#include "zi/omUtility.h"

#include <QTextStream>

std::ostream& operator<<(std::ostream &out, const OmColor& c)
{
    out << "[r" << (int)c.red
        << ",g" << (int)c.green
        << ",b" << (int)c.blue
        << "]";
    return out;
}

std::ostream& operator<<(std::ostream &out, const OmColorRGBA& c)
{
    out << "[r" << (int)c.red
        << ",g" << (int)c.green
        << ",b" << (int)c.blue
        << ",a" << (int)c.alpha
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

QTextStream &operator<<(QTextStream& out, const OmColor& c)
{
    out << c.red << "\t";
    out << c.green << "\t";
    out << c.blue;
    return out;
}

std::ostream& operator<<(std::ostream &out, const OmSegIDsSet& in)
{
    const std::string joined = om::string::join(in);

    out << "[" << joined << "]";
    return out;
}

std::ostream& operator<<(std::ostream &out, const OmToolMode& tool)
{
    switch(tool){
    case SELECT_MODE:
        out << "SELECT_MODE";
        break;
    case PAN_MODE:
        out << "PAN_MODE";
        break;
    case CROSSHAIR_MODE:
        out << "CROSSHAIR_MODE";
        break;
    case ZOOM_MODE:
        out << "ZOOM_MODE";
        break;
    case ADD_VOXEL_MODE:
        out << "ADD_VOXEL_MODE";
        break;
    case SUBTRACT_VOXEL_MODE:
        out << "SUBTRACT_VOXEL_MODE";
        break;
    case FILL_MODE:
        out << "FILL_MODE";
        break;
    case SPLIT_MODE:
        out << "SPLIT_MODE";
        break;
    case CUT_MODE:
        out << "CUT_MODE";
        break;
    default:
        out << "unknown";
        break;
    };

    return out;
}

std::ostream& operator<<(std::ostream &out, const om::CacheGroup& c)
{
    if(om::MESH_CACHE == c){
        out << "MESH_CACHE";
    } else {
        out <<"TILE_CACHE";
    }
    return out;
}
