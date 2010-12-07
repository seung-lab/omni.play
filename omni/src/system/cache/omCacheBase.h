#ifndef OM_CACHE_BASE_H
#define OM_CACHE_BASE_H

/**
 *	Base class of all caches.
 *	Brett Warne - bwarne@mit.edu - 7/15/09
 */

#include "common/omCommon.h"

class OmCacheBase {
public:
	OmCacheBase(OmCacheGroupEnum group)
		: mCacheGroup(group) {}

	virtual ~OmCacheBase(){}

	virtual void UpdateSize(const int64_t delta) = 0;
	virtual int Clean() = 0;
	virtual int GetFetchStackSize() = 0;
	virtual qint64 GetCacheSize() = 0;
	virtual void closeDownThreads() = 0;
	virtual const std::string& GetName() = 0;

	std::string getGroupName()
	{
		if(mCacheGroup == RAM_CACHE_GROUP){
			return "RAM_CACHE";
		}
		return "VRAM_CACHE";
	}

	const OmCacheGroupEnum mCacheGroup;
};

#endif
