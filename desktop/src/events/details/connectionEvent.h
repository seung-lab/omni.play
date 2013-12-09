#pragma once

#include "events/details/event.h"

namespace om {
namespace event {

class ConnectionEvent : public Event {
 public:
  // events
  static const QEvent::Type CONNECTION_CHANGE;

  explicit ConnectionEvent(QEvent::Type type)
      : Event(Klass::connection, type) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<ConnectionEventListener*>(base);
    assert(list);

    if (type_ == CONNECTION_CHANGE) {
      return list->ConnectionChangeEvent();
    }
    throw om::ArgException("unknown event type");
  }
};
}
}  // om::event::
