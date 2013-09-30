#include "events/details/omEventManager.h"
#include "events/details/omEventManagerImpl.hpp"

OmEventManager::OmEventManager() {}

OmEventManager::~OmEventManager() {}

void OmEventManager::AddListener(OmEventClass eventClass,
                                 OmEventListener* listener) {
  impl().AddListener(eventClass, listener);
}

void OmEventManager::RemoveListener(OmEventClass eventClass,
                                    OmEventListener* listener) {
  impl().RemoveListener(eventClass, listener);
}

void OmEventManager::PostEvent(OmEvent* event) { impl().PostEvent(event); }
