#include "threads/omTaskManager.hpp"
#include "threads/omThreadPoolManager.h"

OmThreadPoolManager::~OmThreadPoolManager() { StopAll(); }

void OmThreadPoolManager::StopAll() {
  // as threadpools close down, there are potential races on the pool mutex,
  //  so use copy of pools_

  std::set<om::stoppable*> pools;
  {
    zi::guard g(instance().lock_);
    pools = instance().pools_;
  }

  FOR_EACH(iter, pools) {
    om::stoppable* pool = *iter;

    {
      zi::guard g(instance().lock_);
      if (!instance().pools_.count(pool)) {
        continue;
      }
    }

    pool->StoppableStop();
  }
}

void OmThreadPoolManager::Add(om::stoppable* p) {
  zi::guard g(instance().lock_);
  instance().pools_.insert(p);
}

void OmThreadPoolManager::Remove(om::stoppable* p) {
  zi::guard g(instance().lock_);
  instance().pools_.erase(p);
}
