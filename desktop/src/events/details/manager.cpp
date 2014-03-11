#include "events/details/event.h"
#include "events/details/manager.h"
#include "events/details/managerImpl.hpp"

namespace om {
namespace event {

Manager::Manager() : impl_(new ManagerImpl()) {}
Manager::~Manager() {}

void Manager::AddListener(Klass klass, Listener* listener) {
  instance().impl_->AddListener(klass, listener);
}

void Manager::RemoveListener(Klass klass, Listener* listener) {
  instance().impl_->RemoveListener(klass, listener);
}

void Manager::Post(Event* event) { instance().impl_->Post(event); }
}
}
