#pragma once
#include "precomp.h"

#include "events/details/event.h"
#include "common/logging.h"

namespace om {
namespace event {

class NonFatalEvent : public Event {
 private:
  const QString err_;

 public:
  static const QEvent::Type NON_FATAL_ERROR_OCCURED;

  NonFatalEvent(const QString& err)
      : Event(Klass::nonFatalEvent, NON_FATAL_ERROR_OCCURED), err_(err) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<NonFatalEventListener*>(base);
    assert(list);
    if (!list) {
      log_debugs << "No listeners found";
      return;
    }

    if (type_ == NON_FATAL_ERROR_OCCURED) return list->NonFatalEvent(this);
    throw om::ArgException("unknown event type");
  }

  QString Error() const { return err_; }
};
}
}  // om::event::
