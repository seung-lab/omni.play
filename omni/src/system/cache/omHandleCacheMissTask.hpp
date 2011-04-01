#ifndef OM_HANDLE_CACHE_MISS_TASK_H
#define OM_HANDLE_CACHE_MISS_TASK_H

#include "system/cache/omThreadedCache.h"
#include "zi/omThreads.h"

template <typename KEY, typename PTR >
class OmHandleCacheMissTask : public zi::runnable {
private:
    OmThreadedCache<KEY,PTR> *const mTC;
    const KEY mKey;

public:
    OmHandleCacheMissTask(OmThreadedCache<KEY,PTR>* tc,
                          const KEY& key)
        : mTC(tc)
        , mKey(key)
    {}

    void run()
    {
        if(mTC->killingCache_.get()){
            return;
        }

        PTR val = mTC->HandleCacheMiss(mKey);

        if(mTC->killingCache_.get()){
            return;
        }

        {
            // update cache
            zi::rwmutex::write_guard g(mTC->mutex_);

            if(val){
                mTC->cache_.set(mKey, val);
                mTC->keyAccessList_.touch(mKey);
                if(!mTC->entrySize_){
                    mTC->updateSize(val->NumBytes());
                }
            }
            mTC->currentlyFetching_.erase(mKey);
        }
    }
};

#endif
