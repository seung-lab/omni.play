#ifndef OM_HANDLE_CACHE_MISS_TASK_H
#define OM_HANDLE_CACHE_MISS_TASK_H

#include "system/cache/omThreadedCache.h"
#include "zi/omThreads.h"

template <typename KEY, typename PTR >
class OmHandleCacheMissTask : public zi::runnable {
public:
	OmHandleCacheMissTask(OmThreadedCache<KEY,PTR>* tc,
						  const KEY& key)
		: mTC(tc)
		, mKey(key)
	{}

	void run()
	{
		if(mTC->killingCache_.get()){
			return;
		}

		PTR ret = mTC->HandleCacheMiss(mKey);
		handleFetchUpdate(ret);
	}

private:
	OmThreadedCache<KEY,PTR> *const mTC;
	const KEY mKey;

	inline void handleFetchUpdate(PTR& val)
	{
		if(mTC->killingCache_.get()){
			return;
		}

		doHandleFetchUpdate(val);
	}

	inline void doHandleFetchUpdate(PTR& val)
	{
		zi::guard g(mTC->mutex_);

		if(val){
			mTC->cache_.set(mKey, val);
			mTC->keyAccessList_.touch(mKey);
		}
		mTC->currentlyFetching_.erase(mKey);
	}
};

#endif
