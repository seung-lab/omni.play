#pragma once
#include "precomp.h"

#include "events/details/event.h"
#include "gui/tools.hpp"

namespace om {
namespace event {

class ToolModeEvent : public Event {
 public:
  static const QEvent::Type TOOL_MODE_CHANGE;

  ToolModeEvent(QEvent::Type type, om::tool::mode tool) : Event(Klass::tool, type) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<ToolModeEventListener*>(base);
    assert(list);

    if (type_ == TOOL_MODE_CHANGE) return list->ToolModeChangeEvent(tool_);
    throw om::ArgException("unknown event type");
  }

 private:
  om::tool::mode tool_;
};
}
}  // om::event::
