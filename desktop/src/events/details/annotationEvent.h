#pragma once

#include "events/details/event.h"

namespace om {
namespace event {

class AnnotationEvent : public Event {
 public:
  static const QEvent::Type OBJECT_MODIFIED;

  AnnotationEvent(QEvent::Type type) : Event(Klass::annotation, type) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<AnnotationEventListener*>(base);
    assert(list);

    if (type_ == OBJECT_MODIFIED)
      return list->AnnotationModificationEvent(this);
    throw om::ArgException("unknown event type");
  }
};
}
}  // om::event::
