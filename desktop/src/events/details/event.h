#pragma once
#include "precomp.h"

#include "events/details/enums.hpp"
#include "events/listeners.h"

namespace om {
namespace event {

class Event : public QEvent {
 public:
  const Klass klass_;
  const QEvent::Type type_;

  virtual void Dispatch(Listener*) = 0;

 protected:
  Event(Klass k, QEvent::Type t) : QEvent(t), klass_(k), type_(t) {}
};
}
}  // om::event::
