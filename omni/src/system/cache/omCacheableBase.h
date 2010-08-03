#ifndef OM_CACHEABLE_BASE_H
#define OM_CACHEABLE_BASE_H

/*
 *	Interface for objects used in a GenericCache.  Maintains a pointer to the
 *	cache so UpdateSize() can be called when ever the object changes size.
 *
 *	Brett Warne - bwarne@mit.edu - 6/1/09
 */

class OmCacheBase;


class OmCacheableBase {
 public:
	OmCacheableBase( OmCacheBase *cache );
	//	OmCacheKey GetCacheKey();
	void Remove();
	virtual void Flush() = 0;

 protected:
	void UpdateSize(const qint64 bytesToAdd);

 private:
	OmCacheBase *const mCache;
};

#endif
