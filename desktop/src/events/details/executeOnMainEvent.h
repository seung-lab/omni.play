#pragma once
#include "precomp.h"

#include "events/details/event.h"

namespace om {
namespace event {

class ExecuteOnMainEvent : public Event {
 public:
  static const QEvent::Type EXECUTE_ON_MAIN;

  ExecuteOnMainEvent(std::function<void()> func)
      : Event(Klass::execOnMain, EXECUTE_ON_MAIN), func_(func) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<ExecuteOnMainEventListener*>(base);
    assert(list);

    if (type_ == EXECUTE_ON_MAIN) return list->ExecuteOnMainEvent(this);
    throw om::ArgException("unknown event type");
  }

  std::function<void()> func() const { return func_; }

 private:
  const std::function<void()> func_;
};
}
}  // om::event::
