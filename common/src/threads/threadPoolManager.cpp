#include "threads/taskManager.hpp"
#include "threads/threadPoolManager.h"

namespace om {
namespace thread {

ThreadPoolManager::~ThreadPoolManager() { StopAll(); }

void ThreadPoolManager::StopAll() {
  // as threadpools close down, there are potential races on the pool mutex,
  //  so use copy of pools_

  std::set<common::stoppable*> pools;
  {
    zi::guard g(instance().lock_);
    pools = instance().pools_;
  }

  for (auto* pool : pools) {
    {
      zi::guard g(instance().lock_);
      if (!instance().pools_.count(pool)) {
        continue;
      }
    }

    pool->StoppableStop();
  }
}

void ThreadPoolManager::Add(common::stoppable* p) {
  if (p) {
    zi::guard g(instance().lock_);
    instance().pools_.insert(p);
  }
}

void ThreadPoolManager::Remove(common::stoppable* p) {
  if (p) {
    zi::guard g(instance().lock_);
    instance().pools_.erase(p);
  }
}

}  // namespace threads
}  // namespace om
