#include "omEvent.h"
#include "omEventManager.h"

OmEventListener::OmEventListener(OmEventClass eventClass)
    : eventClass_(eventClass) {
  OmEventManager::AddListener(eventClass, this);
}

OmEventListener::~OmEventListener() {
  OmEventManager::RemoveListener(eventClass_, this);
}
