#ifndef OM_CACHE_BASE_H
#define OM_CACHE_BASE_H

/*
 *	Base class of all caches.  Base class allows
 *	for the CacheManager to organize caches into groups which can be emptied all at once.
 *	UpdateSize() method forwards the update to the manager with appropriate group information.
 *
 *	Brett Warne - bwarne@mit.edu - 7/15/09
 */

#include "omCacheManager.h"

class OmCacheBase {

public:
	//constructor
	OmCacheBase(OmCacheGroup group) {
		mDelayDelta = false;
		mSavedDelta = 0;
		mCacheGroup = group;
		OmCacheManager::AddCache(mCacheGroup, this);
	}

	//destructor
	virtual ~OmCacheBase() {
		OmCacheManager::RemoveCache(mCacheGroup, this);
	}
	
	//update group size
	void UpdateSize(int delta) {
		if (!mDelayDelta) {
			OmCacheManager::UpdateCacheSize(mCacheGroup, delta+mSavedDelta);
			mSavedDelta = 0;
		} else {
			mSavedDelta += delta;
		}
	}
	
	//remove single element from cache
	virtual void RemoveOldest() = 0;
	virtual unsigned int GetFetchStackSize() = 0;
	virtual long GetCacheSize() = 0;

protected:
	int mSavedDelta;
	bool mDelayDelta;
	
	OmCacheGroup mCacheGroup;
};



#endif
