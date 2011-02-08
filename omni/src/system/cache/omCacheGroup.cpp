#include "system/cache/omCacheBase.h"
#include "system/cache/omCacheGroup.h"
#include "system/cache/omCacheInfo.h"
#include "system/cache/omCacheManager.h"
#include "utility/omStringHelpers.h"
#include "zi/omMutex.h"

OmCacheGroup::OmCacheGroup(const om::CacheGroup cacheGroup)
    : cacheGroup_(cacheGroup)
    , maxAllowedSize_(0)
{}

void OmCacheGroup::DeleteCaches()
{
    zi::rwmutex::write_guard lock(lock_);
    caches_.clear();
}

void OmCacheGroup::AddCache(OmCacheBase* cache)
{
    zi::rwmutex::write_guard lock(lock_);
    caches_.insert(cache);
}

void OmCacheGroup::RemoveCache(OmCacheBase* cache)
{
    zi::rwmutex::write_guard lock(lock_);
    caches_.erase(cache);
}

void OmCacheGroup::SetMaxSizeMB(const int64_t size)
{
    zi::rwmutex::write_guard lock(lock_);
    maxAllowedSize_ = size * static_cast<int64_t>(BYTES_PER_MB);
}

QList<OmCacheInfo> OmCacheGroup::GetCacheInfo()
{
    zi::rwmutex::read_guard lock(lock_);

    QList<OmCacheInfo> infos;
    FOR_EACH(iter, caches_){
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
    FOR_EACH(iter, caches_){
        OmCacheBase* cache = *iter;
        cache->Clear();
    }
}


void OmCacheGroup::SignalCachesToCloseDown()
{
    zi::rwmutex::read_guard lock(lock_);
    FOR_EACH(iter, caches_){
        OmCacheBase* cache = *iter;
        cache->closeDownThreads();
    }
}

void OmCacheGroup::clearDeadItems()
{
    // clear old data being held onto by cache; don't remove oldest
    FOR_EACH(iter, caches_){
        OmCacheBase* cache = *iter;
        cache->Clean();
    }
}

int64_t OmCacheGroup::currentSize()
{
    int64_t totalSize = 0;

    FOR_EACH(iter, caches_){
        OmCacheBase* cache = *iter;
        totalSize += cache->GetCacheSize();
    }

    return totalSize;
}

int64_t OmCacheGroup::removeOldest()
{
    static const int numToRemove = 1500;

    int64_t totalSize = 0;

    FOR_EACH(iter, caches_){
        OmCacheBase* cache = *iter;
        cache->RemoveOldest(numToRemove);
        totalSize += cache->GetCacheSize();
    }

    return totalSize;
}

void OmCacheGroup::Clean()
{
    zi::rwmutex::read_guard lock(lock_);

    clearDeadItems();

    const int64_t oldTotalSize = currentSize();

    if(oldTotalSize < maxAllowedSize_){
        return;
    }

    const int64_t curTotalSize = removeOldest();

    if(oldTotalSize != curTotalSize){
        std::cout
            << cacheGroup_
            << " was: " << om::string::bytesToMB(oldTotalSize)
            << "; currently: " << om::string::bytesToMB(curTotalSize)
            << "; max is: " << om::string::bytesToMB(maxAllowedSize_)
            << "\n";
    }
}
