#include "omCacheManager.h"
#include "omCacheBase.h"

#include "common/omDebug.h"
#include "system/events/omPreferenceEvent.h"
#include "system/cache/omCacheInfo.h"
#include "system/omLocalPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"

//init instance pointer
OmCacheManager *OmCacheManager::mspInstance = 0;

/////////////////////////////////
///////
///////          OmStateManager
///////

OmCacheManager::OmCacheManager()
{
	//init vars
	mTargetRatio = 0.99;
	mCurrentlyCleaning = false;

	mDelayDelta = false;
	mSavedDelta = 0;
	mThreadCount = 0;

	doUpdateCacheSizeFromLocalPrefs();
}

OmCacheManager::~OmCacheManager()
{
}

OmCacheManager *OmCacheManager::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmCacheManager();
	}
	return mspInstance;
}

void OmCacheManager::Delete()
{
	if (mspInstance) {
		delete mspInstance;
	}

	mspInstance = NULL;
}

void OmCacheManager::UpdateCacheSizeFromLocalPrefs()
{
	Instance()->doUpdateCacheSizeFromLocalPrefs();
}

void OmCacheManager::doUpdateCacheSizeFromLocalPrefs()
{
	QWriteLocker locker(&mCacheMutex);
	mRamCacheMap.MaxSize =
	    OmLocalPreferences::getRamCacheSizeMB() * float (BYTES_PER_MB);
	mVramCacheMap.MaxSize =
	    OmLocalPreferences::getVRamCacheSizeMB() * float (BYTES_PER_MB);
}

/////////////////////////////////
///////          Accessors Methods

/*
 *	Registers a cache associated with a group with this manager
 */
void OmCacheManager::AddCache(OmCacheGroup group, OmCacheBase * base)
{
	QWriteLocker locker(&Instance()->mCacheMutex);
	if(RAM_CACHE_GROUP == group){
		Instance()->mRamCacheMap.CacheSet.insert(base);
	} else {
		Instance()->mVramCacheMap.CacheSet.insert(base);
	}
}

/*
 *	Removes a registered cache associated with a group from this manager
 */
void OmCacheManager::RemoveCache(OmCacheGroup group, OmCacheBase * base)
{
	QWriteLocker locker(&Instance()->mCacheMutex);

	Instance()->mDelayDelta = true;
	if(RAM_CACHE_GROUP == group){
		Instance()->mRamCacheMap.CacheSet.erase(base);
	} else {
		Instance()->mVramCacheMap.CacheSet.erase(base);
	}
	Instance()->mDelayDelta = false;
}

QList< OmCacheInfo > OmCacheManager::GetCacheInfo(OmCacheGroup group)
{
	QReadLocker locker(&Instance()->mCacheMutex);

	QList< OmCacheInfo > infos;
	
	if(RAM_CACHE_GROUP == group){
		foreach( OmCacheBase * c, Instance()->mRamCacheMap.CacheSet ){
			OmCacheInfo info;
			info.cacheSize = c->GetCacheSize();
			info.cacheName = "fixme";//c->GetCacheName();
			infos << info;
		}
	} else {
		foreach( OmCacheBase * c, Instance()->mVramCacheMap.CacheSet ){
			OmCacheInfo info;
			info.cacheSize = c->GetCacheSize();
			info.cacheName = "fixme";//c->GetCacheName();
			infos << info;
		}
	}
	
	return infos;
}

/*
 *	Static wrapper to update the size of a cache group
 */
void OmCacheManager::UpdateCacheSize(OmCacheGroup group, int delta)
{
	Instance()->UpdateCacheSizeInternal(group, delta);
}

/*
 *	Updates the size of a cache group.  Cleans if cache group size becomes too large.
 */
void OmCacheManager::UpdateCacheSizeInternal(OmCacheGroup group, int delta)
{
	QWriteLocker locker(&Instance()->mCacheMutex);
	if(RAM_CACHE_GROUP == group){
		Instance()->mRamCacheMap.Size += delta;
	} else {
		Instance()->mVramCacheMap.Size += delta;
	}
}

/////////////////////////////////
///////          Cleaning Methods
void OmCacheManager::CleanCacheGroup(OmCacheGroup group)
{
	QReadLocker locker(&Instance()->mCacheMutex);

	//compute target size for group

	for(int count = 0; count < 2; count++) {
		if(RAM_CACHE_GROUP == group){
			const unsigned long target_size = (mTargetRatio) * mRamCacheMap.MaxSize;
			foreach( OmCacheBase * ptr, mRamCacheMap.CacheSet ) {
				if(mRamCacheMap.Size <= target_size){
					return;
				}
				ptr->RemoveOldest();
			}
		} else {
			const unsigned long target_size = (mTargetRatio) * mVramCacheMap.MaxSize;
			foreach( OmCacheBase * ptr, mVramCacheMap.CacheSet ) {
				if(mVramCacheMap.Size <= target_size){
					return;
				}
				ptr->RemoveOldest();
			}
		}
	}
}

unsigned int OmCacheManager::Freshen(bool freshen)
{
	QMutexLocker locker( &Instance()->mFreshnessMutex );

	static unsigned int freshness = 1;

       	if (freshen) {
        	++freshness;
		//printf("freshness:%u\n", freshness);
        }

        return freshness;
}

void OmCacheManager::SignalCachesToCloseDown()
{
	QReadLocker locker(&Instance()->mCacheMutex);

	foreach( OmCacheBase * cache, Instance()->mVramCacheMap.CacheSet ) {
		cache->closeDownThreads();
	}

	foreach( OmCacheBase * cache, Instance()->mRamCacheMap.CacheSet ) {
		cache->closeDownThreads();
	}
}
