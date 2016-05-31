#pragma once
#include "precomp.h"

#include "events/details/event.h"
#include "common/logging.h"

namespace om {
namespace event {

class AnnotationEvent : public Event {
 public:
  static const QEvent::Type OBJECT_MODIFIED;

  AnnotationEvent(QEvent::Type type) : Event(Klass::annotation, type) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<AnnotationEventListener*>(base);
    assert(list);
    if (!list) {
      log_debugs << "No listeners found";
      return;
    }

    if (type_ == OBJECT_MODIFIED)
      return list->AnnotationModificationEvent(this);
    throw om::ArgException("unknown event type");
  }
};
}
}  // om::event::
