#include "system/cache/omHandleCacheMissThreaded.h"
#include "system/cache/omCacheManager.h"

template < typename TC, typename KEY, typename PTR>
HandleCacheMissThreaded<TC, KEY, PTR>::HandleCacheMissThreaded(TC * tc,
							       const KEY & key)
	: mTC(tc)
	, mKey(key)
{
}

template < typename TC, typename KEY, typename PTR >
void HandleCacheMissThreaded<TC, KEY, PTR>::run()
{
	if(mTC->mKillingCache.get()){ return; }

	PTR ret = mTC->HandleCacheMiss(mKey);
	HandleFetchUpdate(ret);
}

template < typename TC, typename KEY, typename PTR>
void HandleCacheMissThreaded<TC, KEY, PTR>::HandleFetchUpdate(PTR val)
{
	if(mTC->mKillingCache.get()){ return; }

	zi::WriteGuard g(mTC->mCacheMutex);

	mTC->mCache.set(mKey, val);
	mTC->mKeyAccessList.touch(mKey);
	mTC->mCurrentlyFetching.erase(mKey);
}
