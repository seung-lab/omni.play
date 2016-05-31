#pragma once
#include "precomp.h"

#include "events/details/event.h"
#include "common/logging.h"

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
    if (!list) {
      log_debugs << "No listeners found";
      return;
    }

    if (type_ == CONNECTION_CHANGE) {
      return list->ConnectionChangeEvent();
    }
    throw om::ArgException("unknown event type");
  }
};
}
}  // om::event::
