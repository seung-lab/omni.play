#include "system/cache/omCacheBase.h"
#include "system/cache/omCacheGroup.h"
#include "system/cache/omCacheInfo.h"
#include "system/cache/omCacheManager.h"
#include "utility/omStringHelpers.h"
#include "zi/omMutex.h"

OmCacheGroup::OmCacheGroup(const om::common::CacheGroup cacheGroup)
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
    maxAllowedSize_ = size * static_cast<int64_t>(om::math::bytesPerMB);
}

std::vector<OmCacheInfo> OmCacheGroup::GetCacheInfo()
{
    zi::rwmutex::read_guard lock(lock_);

    std::vector<OmCacheInfo> infos;
    FOR_EACH(iter, caches_){
        OmCacheBase* cache = *iter;

        OmCacheInfo info;
        info.size = cache->GetCacheSize();

        std::stringstream name;
        name << *cache;
        info.name = name.str();

        infos.push_back(info);
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
        cache->CloseDownThreads();
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

int64_t OmCacheGroup::removeOldest(const int64_t oldTotalSize)
{
    const int64_t totalAmtMemToFreeBytes = oldTotalSize - maxAllowedSize_;

    const int64_t indivAmtMemToFreeBytes =
        totalAmtMemToFreeBytes / caches_.size();

    int64_t totalSize = 0;

    FOR_EACH(iter, caches_){
        OmCacheBase* cache = *iter;
        cache->RemoveOldest(indivAmtMemToFreeBytes);
        totalSize += cache->GetCacheSize();
    }

    return totalSize;
}

void OmCacheGroup::Clean()
{
    zi::rwmutex::read_guard lock(lock_);

    clearDeadItems();

    const int64_t oldTotalSize = currentSize();

    if(oldTotalSize <= maxAllowedSize_){
        return;
    }

    const int64_t curTotalSize = removeOldest(oldTotalSize);

    if(oldTotalSize != curTotalSize){
        std::cout
            << cacheGroup_
            << " was: " << om::string::bytesToMB(oldTotalSize)
            << "; currently: " << om::string::bytesToMB(curTotalSize)
            << "; max is: " << om::string::bytesToMB(maxAllowedSize_)
            << "\n";
    }
}
