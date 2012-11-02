#pragma once

#include "std.h"

namespace om {
namespace common {

/**
 * Color structs
 **/
struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};
std::ostream& operator<<(std::ostream &out, const Color& c);
bool operator<(const Color& a, const Color& b);
bool operator==(const Color& a, const Color& b);

struct ColorARGB {
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};
std::ostream& operator<<(std::ostream &out, const ColorARGB& c);

} // namespace common
} // namespace om