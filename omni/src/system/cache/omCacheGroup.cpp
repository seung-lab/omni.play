#include "system/cache/omCacheBase.h"
#include "system/cache/omCacheGroup.h"
#include "system/cache/omCacheInfo.h"
#include "system/cache/omCacheManager.h"
#include "utility/omStringHelpers.h"
#include "zi/omMutex.h"

OmCacheGroup::OmCacheGroup()
    : mMaxSize(0)
{}

void OmCacheGroup::DeleteCaches()
{
    zi::rwmutex::write_guard lock(lock_);
    mCacheSet.clear();
}

void OmCacheGroup::AddCache(OmCacheBase* cache)
{
    zi::rwmutex::write_guard lock(lock_);
    mCacheSet.insert(cache);
}

void OmCacheGroup::RemoveCache(OmCacheBase* cache)
{
    zi::rwmutex::write_guard lock(lock_);
    mCacheSet.erase(cache);
}

void OmCacheGroup::SetMaxSizeMB(const qint64 size)
{
    zi::rwmutex::write_guard lock(lock_);
    mMaxSize = size * (qint64)BYTES_PER_MB;
}

QList<OmCacheInfo> OmCacheGroup::GetCacheInfo()
{
    zi::rwmutex::read_guard lock(lock_);

    QList<OmCacheInfo> infos;
    FOR_EACH(iter, mCacheSet){
        OmCacheBase* cache = *iter;

        OmCacheInfo info;
        info.size = cache->GetCacheSize();

        std::stringstream name;
        name << *cache;
        info.name = name.str();

        infos << info;
    }
    return infos;
}

void OmCacheGroup::ClearCacheContents()
{
    zi::rwmutex::read_guard lock(lock_);
    FOR_EACH(iter, mCacheSet){
        OmCacheBase* cache = *iter;
        cache->Clear();
    }
}

int OmCacheGroup::Clean()
{
    zi::rwmutex::read_guard lock(lock_);

    uint64_t curSize = 0;
    FOR_EACH(iter, mCacheSet){
        OmCacheBase* cache = *iter;
        curSize += cache->GetCacheSize();
    }

    if(!curSize){
        return 0;
    }

    const uint64_t oldCurSize = curSize;

    int numItemsRemoved = 0;

    // clear old data being held onto by cache; don't remove oldest
    FOR_EACH(iter, mCacheSet){
        OmCacheBase* cache = *iter;

        if(OmCacheManager::AmClosingDown()){
            return numItemsRemoved;
        }

        const uint64_t oldCacheSize = cache->GetCacheSize();
        numItemsRemoved += cache->Clean(false);
        curSize -= (oldCacheSize - cache->GetCacheSize());
    }

    if(oldCurSize != curSize){
        std::cout
            << "currently " << OmStringHelpers::CommaDeliminateNum(curSize) << " bytes;"
            << " was: " << OmStringHelpers::CommaDeliminateNum(oldCurSize) << " bytes;"
            << " max is: " << OmStringHelpers::CommaDeliminateNum(mMaxSize) << "\n";
    }

    if(curSize < mMaxSize){
        return 0;
    }

    // remove oldest items
    static const int numCycles = 200;
    for(int count = 0; count < numCycles; ++count) {
        FOR_EACH(iter, mCacheSet){
            OmCacheBase* cache = *iter;

            if(OmCacheManager::AmClosingDown()){
                return numItemsRemoved;
            }

            uint64_t oldCacheSize = cache->GetCacheSize();
            numItemsRemoved += cache->Clean(true);
            curSize -= (oldCacheSize - cache->GetCacheSize());

            if(curSize < mMaxSize){
                return numItemsRemoved;
            }
        }
    }

    return numItemsRemoved;
}

void OmCacheGroup::SignalCachesToCloseDown()
{
    zi::rwmutex::read_guard lock(lock_);
    FOR_EACH(iter, mCacheSet){
        OmCacheBase* cache = *iter;
        cache->closeDownThreads();
    }
}
