#pragma once

#include "events/details/event.h"

namespace om {
namespace event {

class ToolModeEvent : public Event {
 public:
  static const QEvent::Type TOOL_MODE_CHANGE;

  ToolModeEvent(QEvent::Type type) : Event(Klass::tool, type) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<ToolModeEventListener*>(base);
    assert(list);

    if (type_ == TOOL_MODE_CHANGE) return list->ToolModeChangeEvent();
    throw om::ArgException("unknown event type");
  }
};
}
}  // om::event::
