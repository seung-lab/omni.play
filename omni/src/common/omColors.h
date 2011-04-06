#ifndef OM_COLORS_H
#define OM_COLORS_H

#include "common/omStd.h"

/**
 * color structs
 **/
struct OmColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};
std::ostream& operator<<(std::ostream &out, const OmColor& c);
bool operator<(const OmColor& a, const OmColor& b);

struct OmColorARGB {
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};
std::ostream& operator<<(std::ostream &out, const OmColorARGB& c);

#endif
