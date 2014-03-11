#pragma once

#include "project/omProject.h"
#include "project/omProjectGlobals.h"

namespace om {
namespace event {

class Event;
class Listener;
class ManagerImpl;

class Manager : om::SingletonBase<Manager> {
 public:
  static void AddListener(Klass klass, Listener* listener);
  static void RemoveListener(Klass klass, Listener* listener);
  static void Post(Event* event);

 private:
  Manager();
  ~Manager();

  std::unique_ptr<ManagerImpl> impl_;
  friend class zi::singleton<Manager>;
};
}
}  // om::event::
