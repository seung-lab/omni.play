#include "system/cache/omCacheBase.h"
#include "system/cache/omCacheGroup.h"
#include "system/cache/omCacheInfo.h"
#include "system/cache/omCacheManager.h"
#include "utility/stringHelpers.h"
#include <zi/mutex>

OmCacheGroup::OmCacheGroup()
	: mMaxSize(0)
{
}

void OmCacheGroup::Clear()
{
	zi::WriteGuard lock(mRWLock);
	mCacheSet.clear();
}

void OmCacheGroup::AddCache(OmCacheBase* cache)
{
	zi::WriteGuard lock(mRWLock);
	mCacheSet.insert(cache);
}

void OmCacheGroup::RemoveCache(OmCacheBase* cache)
{
	zi::WriteGuard lock(mRWLock);
	mCacheSet.erase(cache);
}

void OmCacheGroup::SetMaxSizeMB(const qint64 size)
{
	zi::WriteGuard lock(mRWLock);
	mMaxSize = size * (qint64)BYTES_PER_MB;
}

QList<OmCacheInfo> OmCacheGroup::GetCacheInfo()
{
	zi::ReadGuard lock(mRWLock);

	QList<OmCacheInfo> infos;
	foreach(OmCacheBase* c, mCacheSet){
		OmCacheInfo info;
		info.cacheSize = c->GetCacheSize();
		const std::string name =
			c->GetName()
			+ " ("
			+ c->getGroupName()
			+ ")";
		info.cacheName = QString::fromStdString(name);
		infos << info;
	}
	return infos;
}

int OmCacheGroup::Clean()
{
	uint64_t curSize = 0;
	foreach( OmCacheBase * cache, mCacheSet ) {
		curSize += cache->GetCacheSize();
	}

	if(curSize < mMaxSize){
		return 0;
	}

	std::cout << "cur size: "
		  << StringHelpers::commaDeliminateNumber(curSize).toStdString()
		  << " bytes; maxSize is: "
		  << StringHelpers::commaDeliminateNumber(mMaxSize).toStdString()
		  << "\n";

	int numItemsRemoved = 0;
	for(int count = 0; count < 200; count++) {
		foreach( OmCacheBase * cache, mCacheSet ) {
			if(OmCacheManager::AmClosingDown()){
				return numItemsRemoved;
			}

			uint64_t oldCacheSize = cache->GetCacheSize();
			numItemsRemoved += cache->Clean();
			curSize -= (oldCacheSize - cache->GetCacheSize());

			if(curSize < mMaxSize){
				return numItemsRemoved;
			}
		}
	}

	return numItemsRemoved;
}

void OmCacheGroup::SignalCachesToCloseDown()
{
	zi::ReadGuard lock(mRWLock);
	foreach( OmCacheBase * cache, mCacheSet ) {
		cache->closeDownThreads();
	}
}
