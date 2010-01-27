#ifndef OM_CACHEABLE_BASE_H
#define OM_CACHEABLE_BASE_H

/*
 *	Interface for objects used in a GenericCache.  Maintains a pointer to the
 *	cache so UpdateSize() can be called when ever the object changes size.
 *
 *	Brett Warne - bwarne@mit.edu - 6/1/09
 */

#include "omCacheBase.h"


class OmCacheableBase {
public:
	OmCacheableBase( OmCacheBase *cache ) 
	: mCache(cache) { }
	
protected:
	
	/*
	 *	Update the size of the cache.
	 */
	void UpdateSize(int delta) {
		if(mCache) mCache->UpdateSize(delta);
	}
	
private:
	OmCacheBase * const mCache;
};



#endif
