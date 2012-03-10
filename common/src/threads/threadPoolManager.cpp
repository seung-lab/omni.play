#include "threads/taskManager.hpp"
#include "threads/threadPoolManager.h"

namespace om {
namespace threads {

threadPoolManager::~threadPoolManager() {
    StopAll();
}

void threadPoolManager::StopAll()
{
    // as threadpools close down, there are potential races on the pool mutex,
    //  so use copy of pools_

    std::set<common::stoppable*> pools;
    {
        zi::guard g(instance().lock_);
        pools = instance().pools_;
    }

    FOR_EACH(iter, pools)
    {
        common::stoppable* pool = *iter;

        {
            zi::guard g(instance().lock_);
            if(!instance().pools_.count(pool)){
                continue;
            }
        }

        pool->StoppableStop();
    }
}

void threadPoolManager::Add(common::stoppable* p)
{
    zi::guard g(instance().lock_);
    instance().pools_.insert(p);
}

void threadPoolManager::Remove(common::stoppable* p)
{
    zi::guard g(instance().lock_);
    instance().pools_.erase(p);
}

} // namespace threads
} // namespace om
