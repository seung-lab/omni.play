#include "system/omHandleCacheMissThreaded.h"
#include "system/omCacheManager.h"

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
		
		if(mTC->mKillingFetchThread){ 
			return; 
		}
		
		if(mTC->mFetchStack.empty()){
			mTC->mFetchThreadCv.wait(&mTC->mCacheMutex);
		}

		if(mTC->mKillingFetchThread){ 
			return; 
		}
		
		if(mTC->mFetchStack.empty()){
			continue;
		}
		
		mKey = mTC->mFetchStack.top();
		mTC->mFetchStack.pop();
		mTC->mCurrentlyFetching.append(mKey);
		
		lock.unlock();

		OmCacheManager::Instance()->CleanCacheGroup(mTC->mCacheGroup);
		
		PTR * ret = mTC->HandleCacheMiss(mKey);
		HandleFetchUpdate(ret);
	}

}

template < typename TC, typename KEY, typename PTR>
void HandleCacheMissThreaded<TC, KEY, PTR>::HandleFetchUpdate(PTR * val) 
{
	QMutexLocker lock(&mTC->mCacheMutex);

	if(mTC->mKillingFetchThread){
		return;
	}

	//add to cache map
	mTC->mCache[mKey] = QExplicitlySharedDataPointer<PTR>(val);

	//add to access list
	mTC->mKeyAccessList.push_front(mKey);

	//key has been fetched, so remove from currently fetching
	mTC->mCurrentlyFetching.removeAt(mTC->mCurrentlyFetching.indexOf(mKey));
}
