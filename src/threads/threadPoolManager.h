#pragma once

#include "common/omStoppable.h"
#include "common/omCommon.h"
#include "zi/omUtility.h"
#include "zi/omMutex.h"

class threadPoolManager : private om::singletonBase<threadPoolManager> {
private:
    zi::mutex lock_;
    std::set<om::stoppable*> pools_;

public:
    static void StopAll();
    static void Add(om::stoppable*);
    static void Remove(om::stoppable*);

private:
    threadPoolManager()
    {}

    ~threadPoolManager();

    friend class zi::singleton<threadPoolManager>;
};

