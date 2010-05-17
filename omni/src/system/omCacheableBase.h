#ifndef OM_CACHEABLE_BASE_H
#define OM_CACHEABLE_BASE_H

/*
 *	Interface for objects used in a GenericCache.  Maintains a pointer to the
 *	cache so UpdateSize() can be called when ever the object changes size.
 *
 *	Brett Warne - bwarne@mit.edu - 6/1/09
 */

#include "omCacheBase.h"
#include <QSharedData>

class OmCacheableBase : public QSharedData 
{
 public:
	OmCacheableBase( OmCacheBase *cache );	
	virtual void Flush() = 0;
	
 protected:
	void UpdateSize(int delta);
	
 private:
	OmCacheBase * const mCache;
};

#endif
