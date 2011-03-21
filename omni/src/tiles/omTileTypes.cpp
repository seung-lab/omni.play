#include "tiles/omTileTypes.hpp"

std::ostream& operator<<(std::ostream &out, const GLfloatPair& b)
{
    out << "[" << b.x << "," << b.y << "]";
    return out;
}

std::ostream& operator<<(std::ostream &out, const GLfloatBox& b)
{
    out << "lowerLeft: " << b.lowerLeft
        << "\n\t lowerRight: " << b.lowerRight
        << "\n\t upperRight: " << b.upperRight
        << "\n\t upperLeft: " << b.upperLeft
        << "\n";

    return out;
}
