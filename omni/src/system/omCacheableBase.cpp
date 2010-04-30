#include "omCacheableBase.h"

OmCacheableBase::OmCacheableBase( OmCacheBase *cache ) 
	: mCache(cache) 
{ 
}

void OmCacheableBase::UpdateSize(int delta) {
	if(mCache) {
		mCache->UpdateSize(delta);
	}
}
