#include "events/details/event.h"
#include "events/details/manager.h"
#include "events/details/managerImpl.hpp"

namespace om {
namespace event {

void Manager::AddListener(Klass klass, Listener* listener) {
  if (impl()) {
    impl()->AddListener(klass, listener);
  }
}

void Manager::RemoveListener(Klass klass, Listener* listener) {
  if (impl()) {
    impl()->RemoveListener(klass, listener);
  }
}

void Manager::Post(Event* event) {
  if (impl()) {
    impl()->Post(event);
  }
}
}
}
