#pragma once
#include "precomp.h"

#include "events/details/event.h"
#include "common/logging.h"

namespace om {
namespace event {

class MSTEvent : public Event {
 public:
  static const QEvent::Type REFRESH_MST_THRESHOLD;

  MSTEvent() : Event(Klass::mst, REFRESH_MST_THRESHOLD) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<MSTEventListener*>(base);
    assert(list);
    if (!list) {
      log_debugs << "No listeners found";
      return;
    }

    if (type_ == REFRESH_MST_THRESHOLD) return list->RefreshMSTEvent(this);
    throw om::ArgException("unknown event type");
  }
};
}
}  // om::event::
