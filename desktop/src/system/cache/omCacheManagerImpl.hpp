#pragma once

#include "common/common.h"
#include "common/omDebug.h"
#include "system/cache/omCacheBase.h"
#include "system/cache/omCacheGroup.h"
#include "system/cache/omCacheInfo.h"
#include "system/omLocalPreferences.hpp"
#include "utility/omLockedPODs.hpp"
#include "zi/omMutex.h"
#include "zi/omThreads.h"
#include "zi/omUtility.h"
#include "zi/concurrency/periodic_function.hpp"

class OmCacheManagerImpl {
private:
    static const int CLEANER_THREAD_LOOP_TIME_SECS = 30;

public:
    QList<OmCacheInfo> GetCacheInfo(const om::common::CacheGroup group){
        return getCache(group)->GetCacheInfo();
    }

    void ClearCacheContents()
    {
        meshCaches_->ClearCacheContents();
        tileCaches_->ClearCacheContents();
    }

    void AddCache(const om::common::CacheGroup group, OmCacheBase* base){
        getCache(group)->AddCache(base);
    }

    void RemoveCache(const om::common::CacheGroup group, OmCacheBase* base){
        getCache(group)->RemoveCache(base);
    }

    void SignalCachesToCloseDown()
    {
        amClosingDown.set(true);
        cleaner_->stop();

        meshCaches_->SignalCachesToCloseDown();
        tileCaches_->SignalCachesToCloseDown();
    }

    void UpdateCacheSizeFromLocalPrefs()
    {
        meshCaches_->SetMaxSizeMB(OmLocalPreferences::getMeshCacheSizeMB());
        tileCaches_->SetMaxSizeMB(OmLocalPreferences::getTileCacheSizeMB());
    }

    inline void TouchFreshness(){
        freshness_.add(1);
    }

    inline uint64_t GetFreshness(){
        return freshness_.get();
    }

    inline bool AmClosingDown(){
        return amClosingDown.get();
    }

private:
    boost::scoped_ptr<OmCacheGroup> meshCaches_;
    boost::scoped_ptr<OmCacheGroup> tileCaches_;

    boost::shared_ptr<zi::periodic_function> cleaner_;
    boost::shared_ptr<zi::thread> cleanerThread_;

    LockedBool amClosingDown;
    LockedUint64 freshness_;

    OmCacheManagerImpl()
        : meshCaches_(new OmCacheGroup(om::common::MESH_CACHE))
        , tileCaches_(new OmCacheGroup(om::common::TILE_CACHE))
    {
        freshness_.set(1); // non-segmentation tiles have freshness of 0

        meshCaches_->SetMaxSizeMB(OmLocalPreferences::getMeshCacheSizeMB());
        tileCaches_->SetMaxSizeMB(OmLocalPreferences::getTileCacheSizeMB());

        setupCleanerThread();
    }

public:
    ~OmCacheManagerImpl(){
        SignalCachesToCloseDown();
    }

private:
    bool cacheManagerCleaner()
    {
        if(amClosingDown.get()){
            return false;
        }

        meshCaches_->Clean();
        tileCaches_->Clean();

        if(amClosingDown.get()){
            return false;
        }

        return true;
    }

    void setupCleanerThread()
    {
        const int64_t loopTimeSecs= CLEANER_THREAD_LOOP_TIME_SECS;

        cleaner_ =
            boost::make_shared<zi::periodic_function>(
                &OmCacheManagerImpl::cacheManagerCleaner, this,
                zi::interval::secs(loopTimeSecs));

        cleanerThread_ = boost::make_shared<zi::thread>(*cleaner_);
        cleanerThread_->start();
    }

    inline OmCacheGroup* getCache(const om::common::CacheGroup group)
    {
        if(om::common::MESH_CACHE == group){
            return meshCaches_.get();
        }
        return tileCaches_.get();
    }

    friend class OmCacheManager;
};

