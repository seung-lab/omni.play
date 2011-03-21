#include "utility/omThreadPool.hpp"
#include "utility/omThreadPoolManager.h"

OmThreadPoolManager::~OmThreadPoolManager() {
    StopAll();
}

void OmThreadPoolManager::StopAll()
{
    // as threadpools close down, there are potential races on the pool mutex,
    //  so use copy of pools_

    std::set<OmThreadPool*> pools;
    {
        zi::guard g(instance().lock_);
        pools = instance().pools_;
    }

    FOR_EACH(iter, pools)
    {
        OmThreadPool* pool = *iter;

        {
            zi::guard g(instance().lock_);
            if(!instance().pools_.count(pool)){
                continue;
            }
        }

        if(pool->wasStarted()){
            pool->join();
        }
    }
}

void OmThreadPoolManager::Add(OmThreadPool* p)
{
    zi::guard g(instance().lock_);
    instance().pools_.insert(p);
}

void OmThreadPoolManager::Remove(OmThreadPool* p)
{
    zi::guard g(instance().lock_);
    instance().pools_.erase(p);
}
