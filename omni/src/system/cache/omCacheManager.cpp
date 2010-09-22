#include "common/omDebug.h"
#include "system/cache/omCacheBase.h"
#include "system/cache/omCacheGroup.h"
#include "system/cache/omCacheInfo.h"
#include "system/cache/omCacheManager.h"
#include "system/events/omPreferenceEvent.h"
#include "system/omLocalPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"

static const int CLEANER_THREAD_LOOP_TIME_SECS = 30;

OmCacheManager::OmCacheManager()
	: mRamCacheMap(new OmCacheGroup())
	, mVramCacheMap(new OmCacheGroup())
{
	freshness_.set(1); // non-segmentation tiles have freshness of 0

	mRamCacheMap->SetMaxSizeMB(OmLocalPreferences::getRamCacheSizeMB());
	mVramCacheMap->SetMaxSizeMB(OmLocalPreferences::getVRamCacheSizeMB());

	setupCleanerThread();
}

OmCacheManager::~OmCacheManager()
{
}

void OmCacheManager::setupCleanerThread()
{
	const int64_t loopTimeMS = CLEANER_THREAD_LOOP_TIME_SECS * 1000;

	threadFactory_.setDetached(false);

	boost::shared_ptr<zi::Runner>
		threadedCleaner(new zi::Runner(this,
					       &OmCacheManager::cacheManagerCleaner,
					       loopTimeMS));

	threadFactory_.poopThread(threadedCleaner)->start();
}

void OmCacheManager::Delete()
{
	GetCache(RAM_CACHE_GROUP)->Clear();
	GetCache(VRAM_CACHE_GROUP)->Clear();
}

void OmCacheManager::UpdateCacheSizeFromLocalPrefs()
{
	GetCache(RAM_CACHE_GROUP)->
		SetMaxSizeMB(OmLocalPreferences::getRamCacheSizeMB());

	GetCache(VRAM_CACHE_GROUP)->
		SetMaxSizeMB(OmLocalPreferences::getVRamCacheSizeMB());
}

void OmCacheManager::AddCache(const OmCacheGroupEnum group, OmCacheBase* base)
{
	GetCache(group)->AddCache(base);
}

void OmCacheManager::RemoveCache(const OmCacheGroupEnum group, OmCacheBase* base)
{
	GetCache(group)->RemoveCache(base);
}

QList<OmCacheInfo> OmCacheManager::GetCacheInfo(const OmCacheGroupEnum group)
{
	return GetCache(group)->GetCacheInfo();
}

int OmCacheManager::CleanCacheGroup(const OmCacheGroupEnum group)
{
	return GetCache(group)->Clean();
}

void OmCacheManager::SignalCachesToCloseDown()
{
	Instance().amClosingDown.set(true);
	GetCache(RAM_CACHE_GROUP)->SignalCachesToCloseDown();
	GetCache(VRAM_CACHE_GROUP)->SignalCachesToCloseDown();
}

bool OmCacheManager::cacheManagerCleaner()
{
	if(amClosingDown.get()){
		return false;
	}

	int numItemsRemoved = OmCacheManager::CleanCacheGroup(RAM_CACHE_GROUP);
	numItemsRemoved    += OmCacheManager::CleanCacheGroup(VRAM_CACHE_GROUP);

	if( numItemsRemoved > 0 ){
		printf("cleaned cache; removed %d items...\n", numItemsRemoved);
	}

	return true;
}
