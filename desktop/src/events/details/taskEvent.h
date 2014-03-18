#pragma once
#include "precomp.h"

#include "events/details/event.h"

namespace om {
namespace event {

class TaskEvent : public Event {
 public:
  // events
  static const QEvent::Type TASK_CHANGE;
  static const QEvent::Type TASK_STARTED;

  explicit TaskEvent(QEvent::Type type) : Event(Klass::task, type) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<TaskEventListener*>(base);
    assert(list);

    if (type_ == TASK_CHANGE) return list->TaskChangeEvent();
    if (type_ == TASK_STARTED) return list->TaskStartedEvent();
    throw om::ArgException("unknown event type");
  }
};
}
}  // om::event::
