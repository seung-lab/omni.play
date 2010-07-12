#include "system/omHandleCacheMissThreaded.h"

template < typename TC, typename KEY, typename PTR>
HandleCacheMissThreaded<TC, KEY, PTR>::HandleCacheMissThreaded(TC * tc) 
	: mTC(tc)
{
}

template < typename TC, typename KEY, typename PTR >
void HandleCacheMissThreaded<TC, KEY, PTR>::run()
{
	while(1){
		QMutexLocker lock(&mTC->mCacheMutex);

		if(mTC->mKillingFetchThread ||
		   mTC->mFetchStack.empty()){
			mTC->mMaxNumWorkers.release(1);
			return;
		}

		KEY fetch_key = mTC->mFetchStack.top();
		mTC->mFetchStack.pop();
		mTC->mCurrentlyFetching.append(fetch_key);
		
		lock.unlock();

		PTR * ret = mTC->HandleCacheMiss(fetch_key);
		mTC->HandleFetchUpdate(fetch_key, ret);
	}

}
