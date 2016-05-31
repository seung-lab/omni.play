#pragma once
#include "precomp.h"

#include "events/details/event.h"
#include "common/logging.h"

namespace om {
namespace event {

class View2dEvent : public Event {
 public:
  // events
  static const QEvent::Type VIEW_BOX_CHANGE;
  static const QEvent::Type VIEW_CENTER_CHANGE;
  static const QEvent::Type VIEW_POS_CHANGE;
  static const QEvent::Type REDRAW;
  static const QEvent::Type REDRAW_BLOCKING;
  static const QEvent::Type COORD_SYSTEM_CHANGE;

  explicit View2dEvent(QEvent::Type type) : Event(Klass::view2d, type) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<View2dEventListener*>(base);
    assert(list);
    if (!list) {
      log_debugs << "No listeners found";
      return;
    }

    if (type_ == VIEW_BOX_CHANGE) return list->ViewBoxChangeEvent();
    if (type_ == VIEW_CENTER_CHANGE) return list->ViewCenterChangeEvent();
    if (type_ == VIEW_POS_CHANGE) return list->ViewPosChangeEvent();
    if (type_ == REDRAW) return list->ViewRedrawEvent();
    if (type_ == REDRAW_BLOCKING) return list->ViewBlockingRedrawEvent();
    if (type_ == COORD_SYSTEM_CHANGE) return list->CoordSystemChangeEvent();
    throw om::ArgException("unknown event type");
  }
};
}
}  // om::event::
