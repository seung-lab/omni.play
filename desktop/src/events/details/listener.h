#pragma once
#include "precomp.h"

#include "events/details/enums.hpp"

namespace om {
namespace event {

class Listener {
 public:
  const Klass klass_;

 protected:
  Listener(Klass k);
  virtual ~Listener();

 private:
  Listener();
};
}
}  // om::event::
