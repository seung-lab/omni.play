#pragma once

#include "precomp.h"

namespace om {
namespace utility {

class UUID {
 public:
  UUID() : tag_(get()), str_(boost::lexical_cast<std::string>(tag_)) {}

  UUID(const std::string& str) : tag_(get(str)), str_(str) {}

  const std::string& Str() const { return str_; }

  bool operator==(const UUID& other) const { return tag_ == other.tag_; }

  bool operator!=(const UUID& other) const { return !(operator==(other)); }

 private:
  inline boost::uuids::uuid get() {
    static zi::spinlock lock;
    static boost::uuids::random_generator rg;
    zi::guard g(lock);
    return rg();
  }

  inline boost::uuids::uuid get(const std::string& str) {
    static boost::uuids::string_generator sg;
    return sg(str);
  }

  boost::uuids::uuid tag_;
  std::string str_;
};
}
}  // namespace om::utility::
