#include "system/cache/omCacheBase.h"
#include "system/cache/omCacheGroup.h"
#include "system/cache/omCacheInfo.h"
#include "system/cache/omCacheManager.h"
#include "utility/omStringHelpers.h"
#include "zi/omMutex.h"

OmCacheGroup::OmCacheGroup()
	: mMaxSize(0)
{}

void OmCacheGroup::Clear()
{
	zi::rwmutex::write_guard lock(mRWLock);
	mCacheSet.clear();
}

void OmCacheGroup::AddCache(OmCacheBase* cache)
{
	zi::rwmutex::write_guard lock(mRWLock);
	mCacheSet.insert(cache);
}

void OmCacheGroup::RemoveCache(OmCacheBase* cache)
{
	zi::rwmutex::write_guard lock(mRWLock);
	mCacheSet.erase(cache);
}

void OmCacheGroup::SetMaxSizeMB(const qint64 size)
{
	zi::rwmutex::write_guard lock(mRWLock);
	mMaxSize = size * (qint64)BYTES_PER_MB;
}

QList<OmCacheInfo> OmCacheGroup::GetCacheInfo()
{
	zi::rwmutex::read_guard lock(mRWLock);

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

	if(!curSize){
		return 0;
	}

	const uint64_t oldCurSize = curSize;

	int numItemsRemoved = 0;

	// clear old data being held onto by cache; don't remove oldest
	foreach(OmCacheBase* cache, mCacheSet) {
		if(OmCacheManager::AmClosingDown()){
			return numItemsRemoved;
		}

		const uint64_t oldCacheSize = cache->GetCacheSize();
		numItemsRemoved += cache->Clean(false);
		curSize -= (oldCacheSize - cache->GetCacheSize());
	}

	if(oldCurSize != curSize){
		std::cout
			<< "currently " << OmStringHelpers::CommaDeliminateNum(curSize) << " bytes;"
			<< " was: " << OmStringHelpers::CommaDeliminateNum(oldCurSize) << " bytes;"
			<< " max is: " << OmStringHelpers::CommaDeliminateNum(mMaxSize) << "\n";
	}

	if(curSize < mMaxSize){
		return 0;
	}

	// remove oldest items
	static const int numCycles = 200;
	for(int count = 0; count < numCycles; ++count) {
		foreach(OmCacheBase* cache, mCacheSet){
			if(OmCacheManager::AmClosingDown()){
				return numItemsRemoved;
			}

			uint64_t oldCacheSize = cache->GetCacheSize();
			numItemsRemoved += cache->Clean(true);
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
	zi::rwmutex::read_guard lock(mRWLock);
	foreach( OmCacheBase * cache, mCacheSet ) {
		cache->closeDownThreads();
	}
}
