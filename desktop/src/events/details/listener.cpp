#include "listener.h"
#include "manager.h"

namespace om {
namespace event {

Listener::Listener(Klass k) : klass_(k) { Manager::AddListener(klass_, this); }

Listener::~Listener() { Manager::RemoveListener(klass_, this); }
}
}  // om::event::
