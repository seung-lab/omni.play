#pragma once

#include <qapplication.h>
#include <QObject>
#include <QEvent>

#include "common/common.h"
#include "events/details/event.h"
#include "events/details/manager.h"
#include "zi/utility.h"

namespace om {
namespace event {

class ManagerImpl : public QObject {
 private:
  typedef std::set<Listener*> listeners_t;

  std::array<listeners_t, Klass::NUM_TOTAL> listenersByKlass_;
  zi::spinlock lock_;

 public:
  void AddListener(Klass klass, Listener* listener) {
    zi::guard g(lock_);
    listenersByKlass_[klass].insert(listener);
  }

  void RemoveListener(Klass klass, Listener* listener) {
    zi::guard g(lock_);
    listenersByKlass_[klass].erase(listener);
  }

  inline void Post(Event* event) { QCoreApplication::postEvent(this, event); }

 private:
  // qtevent handler for custom event
  bool event(QEvent* qevent) {
    auto* event = dynamic_cast<Event*>(qevent);
    auto klass = event->klass_;

    // get a copy of listeners
    listeners_t listeners;
    {
      zi::guard g(lock_);
      listeners = listenersByKlass_[klass];
    }

    if (listeners.empty()) {
      return true;
    }

    for (auto* listener : listeners) {
      event->Dispatch(listener);
    }

    return true;
  }
};
}
}  // om::event::
