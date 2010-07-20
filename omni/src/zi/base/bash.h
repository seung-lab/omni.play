/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef COMMON_BASE_BASH_H_
#define COMMON_BASE_BASH_H_

#include <iostream>
#include <string>

namespace zi { namespace Bash {

#define DEF_COLOR(_COLOR_, _STRING_)                                    \
  static const std::string _COLOR_ = _STRING_

DEF_COLOR(BLACK,       "\033[22;30m");
DEF_COLOR(RED,         "\033[22;31m");
DEF_COLOR(GREEN,       "\033[22;32m");
DEF_COLOR(BROWN,       "\033[22;33m");
DEF_COLOR(BLUE,        "\033[22;34m");
DEF_COLOR(MAGENTA,     "\033[22;35m");
DEF_COLOR(CYAN,        "\033[22;36m");
DEF_COLOR(GRAY,        "\033[22;37m");
DEF_COLOR(DARKGRAY,    "\033[01;30m");
DEF_COLOR(LITERED,     "\033[01;31m");
DEF_COLOR(LITEGREEN,   "\033[01;32m");
DEF_COLOR(YELLOW,      "\033[01;33m");
DEF_COLOR(LITEBLUE,    "\033[01;34m");
DEF_COLOR(LITEMAGENTA, "\033[01;35m");
DEF_COLOR(LITECYAN,    "\033[01;36m");
DEF_COLOR(WHITE,       "\033[01;37m");

#undef DEF_COLOR

} }


#endif
