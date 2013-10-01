#pragma once

#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "zi/utility.h"

namespace om {
namespace event {

class Event;
class Listener;
class ManagerImpl;

class Manager : om::SingletonBase<Manager> {
 private:
  inline static ManagerImpl* impl() {
    if (!OmProject::IsOpen()) {
      return nullptr;
    }
    return &OmProject::Globals().EventManImpl();
  }

 public:
  static void AddListener(Klass klass, Listener* listener);
  static void RemoveListener(Klass klass, Listener* listener);
  static void Post(Event* event);

 private:
  Manager() {}
  ~Manager() {}

  friend class zi::singleton<Manager>;
};
}
}  // om::event::
