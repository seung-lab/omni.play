#pragma once
#include "precomp.h"

#include "events/details/event.h"
#include "common/logging.h"

namespace om {
namespace event {

class UserSettingsUpdatedEvent : public Event {
 public:
  static const QEvent::Type USER_SETTINGS_UPDATED;

  UserSettingsUpdatedEvent() : Event(Klass::userSettings, USER_SETTINGS_UPDATED) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<UserSettingsUpdatedEventListener*>(base);
    assert(list);
    if (!list) {
      log_debugs << "No listeners found";
      return;
    }

    if (type_ == USER_SETTINGS_UPDATED) return list->UserSettingsUpdatedEvent(this);
    throw om::ArgException("unknown event type");
  }
};
}
}  // om::event::
