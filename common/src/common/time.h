#pragma once

#include "boost/date_time/posix_time/posix_time.hpp"

namespace om {
namespace time {
using time = boost::posix_time::ptime;
using duration = boost::posix_time::time_duration;
using boost::posix_time::time_from_string;
using boost::posix_time::to_simple_string;
}
}

namespace YAML {

template <>
struct convert<om::time::time> {
  static Node encode(const om::time::time& t) {
    return Node(om::time::to_simple_string(t));
  }

  static bool decode(const Node& node, om::time::time& c) {
    try {
      c = om::time::time_from_string(node.as<std::string>());
      return true;
    }
    catch (YAML::Exception e) {
      log_debugs(YAML) << std::string("Error Decoding time: ") + e.what();
      return false;
    }
  }
};
}