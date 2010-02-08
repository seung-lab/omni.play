
#include "omCacheManager.h"
#include "omCacheBase.h"

#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omDebug.h"

#define DEBUG 0

//init instance pointer
OmCacheManager *OmCacheManager::mspInstance = 0;

#pragma mark -
#pragma mark OmStateManager
/////////////////////////////////
///////
///////          OmStateManager
///////

OmCacheManager::OmCacheManager()
{
	//init mutex
	pthread_mutex_init(&mCacheMapMutex, NULL);
	pthread_mutex_init(&mRealCacheMapMutex, NULL);

	//init vars
	mTargetRatio = 0.25;
	mCurrentlyCleaning = false;

	mDelayDelta = false;
	mSavedDelta = 0;
	mThreadCount = 0;

	//refresh prefs to call event listener
	mCacheMap[RAM_CACHE_GROUP].MaxSize =
	    OmPreferences::GetFloat(OM_PREF_SYSTEM_RAM_GROUP_CACHE_MAX_MB_FLT) * float (BYTES_PER_MB);
	mCacheMap[VRAM_CACHE_GROUP].MaxSize =
	    OmPreferences::GetFloat(OM_PREF_SYSTEM_VRAM_GROUP_CACHE_MAX_MB_FLT) * float (BYTES_PER_MB);
}

OmCacheManager::~OmCacheManager()
{

}

OmCacheManager *OmCacheManager::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmCacheManager;
	}
	return mspInstance;
}

void OmCacheManager::Delete()
{
	if (mspInstance)
		delete mspInstance;
	mspInstance = NULL;
}

#pragma mark
#pragma mark Event Listener Methods
/////////////////////////////////
///////          Event Listener Methods

/*
 *	Preferences listener for cache group size changes.
 */
void OmCacheManager::PreferenceChangeEvent(OmPreferenceEvent * event)
{
	//debug("FIXME", << "OmCacheManager::PreferenceChangeEvent" << endl;
	switch (event->GetPreference()) {

	case OM_PREF_SYSTEM_RAM_GROUP_CACHE_MAX_MB_FLT:
		mCacheMap[RAM_CACHE_GROUP].MaxSize =
		    OmPreferences::GetFloat(OM_PREF_SYSTEM_RAM_GROUP_CACHE_MAX_MB_FLT) * float (BYTES_PER_MB);
		break;

	case OM_PREF_SYSTEM_VRAM_GROUP_CACHE_MAX_MB_FLT:
		mCacheMap[VRAM_CACHE_GROUP].MaxSize =
		    OmPreferences::GetFloat(OM_PREF_SYSTEM_VRAM_GROUP_CACHE_MAX_MB_FLT) * float (BYTES_PER_MB);
		break;

	default:
		return;
	}

}

#pragma mark
#pragma mark Accessors Methods
/////////////////////////////////
///////          Accessors Methods

/*
 *	Registers a cache associated with a group with this manager
 */
void OmCacheManager::AddCache(OmCacheGroup group, OmCacheBase * base)
{
	pthread_mutex_lock(&Instance()->mCacheMapMutex);
	pthread_mutex_lock(&Instance()->mRealCacheMapMutex);
	Instance()->mCacheMap[group].CacheSet.insert(base);
	pthread_mutex_unlock(&Instance()->mRealCacheMapMutex);
	pthread_mutex_unlock(&Instance()->mCacheMapMutex);
}

/*
 *	Removes a registered cache associated with a group from this manager
 */
void OmCacheManager::RemoveCache(OmCacheGroup group, OmCacheBase * base)
{
	pthread_mutex_lock(&Instance()->mCacheMapMutex);
	pthread_mutex_lock(&Instance()->mRealCacheMapMutex);
	Instance()->mDelayDelta = true;
	Instance()->mCacheMap[group].CacheSet.erase(base);
	Instance()->mDelayDelta = false;
	pthread_mutex_unlock(&Instance()->mRealCacheMapMutex);
	pthread_mutex_unlock(&Instance()->mCacheMapMutex);
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

	pthread_mutex_lock(&mRealCacheMapMutex);
	Instance()->mCacheMap[group].Size += delta;
	pthread_mutex_unlock(&mRealCacheMapMutex);

}

#pragma mark
#pragma mark Cleaning Methods
/////////////////////////////////
///////          Cleaning Methods

void OmCacheManager::CleanCacheGroup(OmCacheGroup group)
{
	pthread_mutex_lock(&mCacheMapMutex);

	//compute target size for group
	uint32_t target_size = mTargetRatio * mCacheMap[group].MaxSize;
	//debug ("thread", "target_size:%i = mTargetRatio:%f * mCacheMap[%i].MaxSize = %i, %i\n",
	//		  target_size, mTargetRatio, group, mCacheMap[group].MaxSize, mCacheMap[group].Size);

	//get caches in group
	set < OmCacheBase * >&cache_set = mCacheMap[group].CacheSet;

	//note last successful remove cache
	OmCacheBase *p_last_successful_remove_cache = NULL;
	//call event for every listener in the set
	set < OmCacheBase * >::iterator itr;
	//loop through set
	//while (target_size) {
	for (int count = 0; count < 2; count++) {
		for (itr = cache_set.begin(); itr != cache_set.end(); ++itr) {

			//return if target reached
			if (mCacheMap[group].Size <= target_size)
				goto end_cleaning_group;

			//remove element from cache
			bool removed = (*itr)->RemoveOldest();

			//debug ("thread", "target_size:%i = mTargetRatio:%f * mCacheMap[%i].MaxSize = %i\n",
			// 		target_size, mTargetRatio, group, mCacheMap[group].MaxSize);

			//if successfully removed
			if (removed) {
				//debug ("thread", "removed\n");
				//record element was removed from cache
				p_last_successful_remove_cache = *itr;
			} else {
				//debug ("thread", "not removed\n");
				//check if we have looped unsuccessfully
				if (p_last_successful_remove_cache == *itr)
					goto end_cleaning_group;
			}
		}
	}

 end_cleaning_group:

	pthread_mutex_unlock(&mCacheMapMutex);
}

void *OmCacheManager::CleanOne(void *in)
{
	OmCacheBase *base = (OmCacheBase *) in;

	//debug("FIXME", << "in......................................: " << base << endl;
	base->RemoveOldest();
	pthread_mutex_lock(&Instance()->mCacheMapMutex);
	Instance()->mThreadCount--;
	pthread_mutex_unlock(&Instance()->mCacheMapMutex);
}
