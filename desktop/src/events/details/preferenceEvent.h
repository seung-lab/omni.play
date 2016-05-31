#pragma once
#include "precomp.h"

#include "events/details/event.h"
#include "common/logging.h"

namespace om {
namespace event {

class PreferenceEvent : public Event {
 public:
  static const QEvent::Type PREFERENCE_CHANGE;

  PreferenceEvent(QEvent::Type type, int pref)
      : Event(Klass::preference, type), preference_(pref) {}

  void Dispatch(Listener* base) {
    auto* list = dynamic_cast<PreferenceEventListener*>(base);
    assert(list);
    if (!list) {
      log_debugs << "No listeners found";
      return;
    }

    if (type_ == PREFERENCE_CHANGE) return list->PreferenceChangeEvent(this);
    throw om::ArgException("unknown event type");
  }

  int GetPreference() { return preference_; }

 private:
  const int preference_;
};
}
}  // om::event::
