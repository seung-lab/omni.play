#pragma once

#include <boost/date_time.hpp>

using boost::posix_time;

namespace om {
namespace datetime {

std::string cur() {
  ptime now = second_clock::universal_time();
  return to_iso_extended_string(now);
}

}  // namespace datetime
}  // namespace om
