#pragma once

#include "std.h"

namespace om {
namespace common {

/**
 * color structs
 **/
struct color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};
std::ostream& operator<<(std::ostream& out, const color& c);
bool operator<(const color& a, const color& b);
bool operator==(const color& a, const color& b);

struct colorARGB {
  uint8_t alpha;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};
std::ostream& operator<<(std::ostream& out, const colorARGB& c);

}  // namespace common
}  // namespace om