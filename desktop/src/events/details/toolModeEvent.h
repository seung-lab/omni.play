#pragma once
#include "precomp.h"

#include "events/details/event.h"
#include "gui/tools.hpp"
#include "common/logging.h"

namespace om {
namespace event {

class ToolModeEvent : public Event {
 public:
  static const QEvent::Type TOOL_MODE_CHANGE;

  ToolModeEvent(QEvent::Type type, om::tool::mode tool)
    : Event(Klass::tool, type), tool_(tool) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<ToolModeEventListener*>(base);
    assert(list);
    if (!list) {
      log_debugs << "No listeners found";
      return;
    }

    if (type_ == TOOL_MODE_CHANGE) return list->ToolModeChangeEvent(tool_);
    throw om::ArgException("unknown event type");
  }

 private:
  om::tool::mode tool_;
};
}
}  // om::event::
