#include "omCacheBase.h"
#include "omCacheableBase.h"

OmCacheableBase::OmCacheableBase( OmCacheBase *cache )
	: mCache(cache)
{
	assert(mCache);
}

void OmCacheableBase::UpdateSize(const qint64 bytesToAdd)
{
	mCache->UpdateSize(bytesToAdd);
}
