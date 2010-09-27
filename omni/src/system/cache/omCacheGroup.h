#ifndef OM_CACHE_GROUP_H
#define OM_CACHE_GROUP_H

#include "common/omCommon.h"

#include <QList>
#include "zi/omMutex.h"

class OmCacheBase;
class OmCacheInfo;

class OmCacheGroup {
public:
	OmCacheGroup();

	void AddCache(OmCacheBase* cache);
	void RemoveCache(OmCacheBase* cache);
	void Clear();

	void SetMaxSizeMB(const qint64 size);

	int Clean();
	void SignalCachesToCloseDown();

	QList<OmCacheInfo> GetCacheInfo();

private:
	uint64_t mMaxSize;
	zi::rwmutex mRWLock;
	std::set<OmCacheBase*> mCacheSet;
};

#endif
