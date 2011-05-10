#pragma once

#include "common/omStoppable.h"
#include "common/omCommon.h"
#include "zi/omUtility.h"
#include "zi/omMutex.h"

class OmThreadPoolManager : private om::singletonBase<OmThreadPoolManager> {
private:
    zi::mutex lock_;
    std::set<om::stoppable*> pools_;

public:
    static void StopAll();
    static void Add(om::stoppable*);
    static void Remove(om::stoppable*);

private:
    OmThreadPoolManager()
    {}

    ~OmThreadPoolManager();

    friend class zi::singleton<OmThreadPoolManager>;
};

