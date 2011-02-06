#ifndef OM_CACHE_MANAGER_IMPL_HPP
#define OM_CACHE_MANAGER_IMPL_HPP

#include "common/omCommon.h"
#include "common/omDebug.h"
#include "system/cache/omCacheBase.h"
#include "system/cache/omCacheGroup.h"
#include "system/cache/omCacheInfo.h"
#include "system/cache/omCacheManager.h"
#include "events/omPreferenceEvent.h"
#include "system/omLocalPreferences.hpp"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "utility/omLockedPODs.hpp"
#include "zi/omMutex.h"
#include "zi/omThreads.h"
#include "zi/omUtility.h"
#include "zi/concurrency/periodic_function.hpp"

class OmCacheManagerImpl {
private:
	static const int CLEANER_THREAD_LOOP_TIME_SECS = 30;

public:
	QList<OmCacheInfo> GetCacheInfo(const OmCacheGroupEnum group){
		return getCache(group)->GetCacheInfo();
	}

	void AddCache(const OmCacheGroupEnum group, OmCacheBase* base){
		getCache(group)->AddCache(base);
	}

	void RemoveCache(const OmCacheGroupEnum group, OmCacheBase* base){
		getCache(group)->RemoveCache(base);
	}

	void SignalCachesToCloseDown()
	{
		amClosingDown.set(true);
		cleaner_->stop();
		mRamCacheMap->SignalCachesToCloseDown();
		mVramCacheMap->SignalCachesToCloseDown();
	}

	void UpdateCacheSizeFromLocalPrefs()
	{
		mRamCacheMap->SetMaxSizeMB(OmLocalPreferences::getRamCacheSizeMB());
		mVramCacheMap->SetMaxSizeMB(OmLocalPreferences::getVRamCacheSizeMB());
	}

	inline void TouchFresheness(){
		freshness_.add(1);
	}

	inline uint64_t GetFreshness(){
		return freshness_.get();
	}

	inline bool AmClosingDown(){
		return amClosingDown.get();
	}

private:
	OmCacheManagerImpl()
		: mRamCacheMap(new OmCacheGroup())
		, mVramCacheMap(new OmCacheGroup())
	{
		freshness_.set(1); // non-segmentation tiles have freshness of 0

		mRamCacheMap->SetMaxSizeMB(OmLocalPreferences::getRamCacheSizeMB());
		mVramCacheMap->SetMaxSizeMB(OmLocalPreferences::getVRamCacheSizeMB());

		setupCleanerThread();
	}

public:
	~OmCacheManagerImpl(){
		SignalCachesToCloseDown();
	}

private:
	boost::shared_ptr<OmCacheGroup> mRamCacheMap;
	boost::shared_ptr<OmCacheGroup> mVramCacheMap;
	boost::shared_ptr<zi::periodic_function> cleaner_;
	boost::shared_ptr<zi::thread> cleanerThread_;
	LockedBool amClosingDown;
	LockedUint64 freshness_;

	bool cacheManagerCleaner()
	{
		if(amClosingDown.get()){
			return false;
		}

		const int numItemsRemoved =
			mRamCacheMap->Clean() + mVramCacheMap->Clean();

		if(numItemsRemoved > 0){
			printf("\tcleaned cache; removed %d items...\n", numItemsRemoved);
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

	inline boost::shared_ptr<OmCacheGroup>&
	getCache(const OmCacheGroupEnum group)
	{
		if(RAM_CACHE_GROUP == group){
			return mRamCacheMap;
		}
		return mVramCacheMap;
	}

	friend class OmCacheManager;
};

#endif
