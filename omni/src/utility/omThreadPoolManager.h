#ifndef OM_THREAD_POOL_MANAGER_HPP
#define OM_THREAD_POOL_MANAGER_HPP

#include "common/omCommon.h"
#include "zi/omUtility.h"
#include "zi/omMutex.h"

class OmThreadPool;

class OmThreadPoolManager : private om::singletonBase<OmThreadPoolManager> {
private:
    zi::mutex lock_;
    std::set<OmThreadPool*> pools_;

public:
    static void StopAll();
    static void Add(OmThreadPool*);
    static void Remove(OmThreadPool*);

private:
    OmThreadPoolManager()
    {}

    ~OmThreadPoolManager();

    friend class zi::singleton<OmThreadPoolManager>;
};

#endif
