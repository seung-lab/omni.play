#ifndef OM_HANDLE_CACHE_MISS_THREADED_H
#define OM_HANDLE_CACHE_MISS_THREADED_H

#include "zi/omThreads.h"

template <typename KEY, typename PTR >
class HandleCacheMissThreaded : public zi::runnable {
public:
	HandleCacheMissThreaded(OmThreadedCache<KEY,PTR>* tc,
							const KEY& key)
		: mTC(tc)
		, mKey(key)
	{}

	void run()
	{
		if(mTC->mKillingCache.get()){
			return;
		}

		PTR ret = mTC->HandleCacheMiss(mKey);
		HandleFetchUpdate(ret);
	}

private:
	OmThreadedCache<KEY,PTR>* const mTC;
	KEY mKey;

	void HandleFetchUpdate(PTR val)
	{
		if(mTC->mKillingCache.get()){
			return;
		}

		doHandleFetchUpdate(val);
	}

	void doHandleFetchUpdate(PTR val)
	{
		zi::guard g(mTC->mutex_);

		if(val){
			mTC->mCache.set(mKey, val);
			mTC->mKeyAccessList.touch(mKey);
		}
		mTC->mCurrentlyFetching.erase(mKey);
	}
};

#endif
