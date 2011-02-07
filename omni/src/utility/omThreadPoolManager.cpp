#include "utility/omThreadPool.hpp"
#include "utility/omThreadPoolManager.h"

void OmThreadPoolManager::StopAll()
{
    zi::guard g(instance().lock_);

    FOR_EACH(iter, instance().pools_){
        OmThreadPool* pool = *iter;
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
