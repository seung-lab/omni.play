#pragma once
#include "precomp.h"

#include "events/details/event.h"
#include "common/logging.h"

namespace om {
namespace event {

class View3dEvent : public Event {
 public:
  static const QEvent::Type REDRAW;
  static const QEvent::Type RECENTER;

  explicit View3dEvent(QEvent::Type type) : Event(Klass::view3d, type) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<View3dEventListener*>(base);
    assert(list);
    if (!list) {
      log_debugs << "No listeners found";
      return;
    }

    if (type_ == REDRAW) return list->View3dRedrawEvent();
    if (type_ == RECENTER) return list->View3dRecenter();
    throw om::ArgException("unknown event type");
  }
};
}
}  // om::event::
