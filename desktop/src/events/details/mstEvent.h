#pragma once

#include "events/details/event.h"

namespace om {
namespace event {

class MSTEvent : public Event {
 public:
  static const QEvent::Type REFRESH_MST_THRESHOLD;

  MSTEvent() : Event(Klass::mst, REFRESH_MST_THRESHOLD) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<MSTEventListener*>(base);
    assert(list);

    if (type_ == REFRESH_MST_THRESHOLD) return list->RefreshMSTEvent(this);
    throw om::ArgException("unknown event type");
  }
};
}
}  // om::event::
