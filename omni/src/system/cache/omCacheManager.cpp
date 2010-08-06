#include "omCacheManager.h"
#include "omCacheBase.h"

#include "system/cache/omCacheGroup.h"
#include "common/omDebug.h"
#include "system/events/omPreferenceEvent.h"
#include "system/cache/omCacheInfo.h"
#include "system/omLocalPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"

OmCacheManager::OmCacheManager()
	: mRamCacheMap(new OmCacheGroup())
	, mVramCacheMap(new OmCacheGroup())
{
	mRamCacheMap->SetMaxSizeMB(OmLocalPreferences::getRamCacheSizeMB());
	mVramCacheMap->SetMaxSizeMB(OmLocalPreferences::getVRamCacheSizeMB());

	const int64_t loopTimeSecs = 30;
	const int64_t loopTimeMS = loopTimeSecs * 1000;

	threadFactory_.setDetached(false);
	threadFactory_.poopThread(boost::shared_ptr<zi::Runner>
				  (new zi::Runner(this,
						  &OmCacheManager::CacheManagerCleaner, loopTimeMS)))->start();
}

OmCacheManager::~OmCacheManager()
{
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

void OmCacheManager::AddCache(OmCacheGroupEnum group, OmCacheBase * base)
{
	GetCache(group)->AddCache(base);
}

void OmCacheManager::RemoveCache(OmCacheGroupEnum group, OmCacheBase * base)
{
	GetCache(group)->RemoveCache(base);
}

QList<OmCacheInfo> OmCacheManager::GetCacheInfo(OmCacheGroupEnum group)
{
	return GetCache(group)->GetCacheInfo();
}

int OmCacheManager::CleanCacheGroup(OmCacheGroupEnum group)
{
	return GetCache(group)->Clean();
}

void OmCacheManager::SignalCachesToCloseDown()
{
	Instance().amClosingDown.set(true);
	GetCache(RAM_CACHE_GROUP)->SignalCachesToCloseDown();
	GetCache(VRAM_CACHE_GROUP)->SignalCachesToCloseDown();
}

unsigned int OmCacheManager::Freshen(const bool freshen)
{
	static zi::Mutex mutex;
	static unsigned int freshness = 1;
	{
		zi::Guard g(mutex);
		if (freshen) ++freshness;
		//printf("freshness:%u\n", freshness);
		return freshness;
	}
}

bool OmCacheManager::CacheManagerCleaner()
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
