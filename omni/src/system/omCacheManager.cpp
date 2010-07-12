#include "omCacheManager.h"
#include "omCacheBase.h"

#include "common/omDebug.h"
#include "system/events/omPreferenceEvent.h"
#include "system/omCacheInfo.h"
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
	mTargetRatio = 0.75;
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
	mCacheMap[RAM_CACHE_GROUP].MaxSize =
	    OmLocalPreferences::getRamCacheSizeMB() * float (BYTES_PER_MB);
	mCacheMap[VRAM_CACHE_GROUP].MaxSize =
	    OmLocalPreferences::getVRamCacheSizeMB() * float (BYTES_PER_MB);
}

/////////////////////////////////
///////          Accessors Methods

/*
 *	Registers a cache associated with a group with this manager
 */
void OmCacheManager::AddCache(OmCacheGroup group, OmCacheBase * base)
{
	Instance()->mCacheMapMutex.lock();
	Instance()->mRealCacheMapMutex.lock();
	Instance()->mCacheMap[group].CacheSet.insert(base);
	Instance()->mRealCacheMapMutex.unlock();
	Instance()->mCacheMapMutex.unlock();
}

/*
 *	Removes a registered cache associated with a group from this manager
 */
void OmCacheManager::RemoveCache(OmCacheGroup group, OmCacheBase * base)
{
	Instance()->mCacheMapMutex.lock();
	Instance()->mRealCacheMapMutex.lock();
	Instance()->mDelayDelta = true;
	Instance()->mCacheMap[group].CacheSet.erase(base);
	Instance()->mDelayDelta = false;
	Instance()->mRealCacheMapMutex.unlock();
	Instance()->mCacheMapMutex.unlock();
}

QList< OmCacheInfo > OmCacheManager::GetCacheInfo(OmCacheGroup group)
{
	// TODO: mutex lock
	//QMutexLocker locker( &mCacheMapMutex );

	QList< OmCacheInfo > infos;
	
	foreach( OmCacheBase * c, Instance()->mCacheMap[group].CacheSet ){
		OmCacheInfo info;
		info.cacheSize = c->GetCacheSize();
		info.cacheName = "fixme";//c->GetCacheName();
		infos << info;
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
	QMutexLocker locker( &mRealCacheMapMutex );
	Instance()->mCacheMap[group].Size += delta;
}

/////////////////////////////////
///////          Cleaning Methods
void OmCacheManager::CleanCacheGroup(OmCacheGroup group)
{
	QMutexLocker locker( &mCacheMapMutex );

	//compute target size for group
	unsigned long target_size = (mTargetRatio) * mCacheMap[group].MaxSize;

	for (int count = 0; count < 2; count++) {
		foreach( OmCacheBase * ptr, mCacheMap[group].CacheSet ) {
			
			//return if target reached
			if (mCacheMap[group].Size <= target_size){
				return;
			}
			
			ptr->RemoveOldest();
		}
	}
}

unsigned int OmCacheManager::Freshen(bool freshen)
{
	QMutexLocker locker( &Instance()->mFreshnessMutex );

	static unsigned int freshness = 1;

       	if (freshen) {
        	++freshness;
        }

        return freshness;
}

bool OmCacheManager::addWorkerThread(QRunnable * runnable, int priority)
{
	return Instance()->threads.tryStart(runnable);
}

void OmCacheManager::clearWorkerThreads()
{
	Instance()->threads.waitForDone();
}
