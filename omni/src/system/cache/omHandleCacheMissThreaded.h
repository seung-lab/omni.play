#ifndef OM_HANDLE_CACHE_MISS_THREADED_H
#define OM_HANDLE_CACHE_MISS_THREADED_H

#include <zi/threads>

template <typename KEY, typename PTR >
class HandleCacheMissThreaded : public zi::Runnable {
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
		zi::Guard g(mTC->mCacheMutex);

		if(val){
			mTC->mCache.set(mKey, val);
			mTC->mKeyAccessList.touch(mKey);
		}
		mTC->mCurrentlyFetching.erase(mKey);
	}
};

#endif
