#pragma once
#include "precomp.h"

// based on http://www.boost.org/doc/libs/1_42_0/libs/uuid/uuid.html

namespace om {
namespace uuid_ {

// random-number generator is not thread-safe, so wrap w/ spinlock
inline boost::uuids::uuid get() {
  static zi::spinlock lock;
  static boost::uuids::random_generator rg;

  zi::guard g(lock);
  return rg();
}

struct idAndStr {
  boost::uuids::uuid tag_;
  std::string str;
};

inline idAndStr makePair() {
  const boost::uuids::uuid u = get();
  idAndStr ret = {u, boost::lexical_cast<std::string>(u)};
  return ret;
}

}  // namespace uuid_
}  // namespace om

class OmUUID {
 private:
  const om::uuid_::idAndStr id;

 public:
  OmUUID() : id(om::uuid_::makePair()) {}

  const std::string& Str() const { return id.str; }
};
