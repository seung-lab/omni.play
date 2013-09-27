#pragma once

#include "common/stoppable.h"
#include "common/common.h"
#include "zi/omUtility.h"
#include "zi/omMutex.h"

class OmThreadPoolManager : private om::singletonBase<OmThreadPoolManager> {
private:
    zi::mutex lock_;
    std::set<om::common::stoppable*> pools_;

public:
    static void StopAll();
    static void Add(om::common::stoppable*);
    static void Remove(om::common::stoppable*);

private:
    OmThreadPoolManager()
    {}

    ~OmThreadPoolManager();

    friend class zi::singleton<OmThreadPoolManager>;
};

