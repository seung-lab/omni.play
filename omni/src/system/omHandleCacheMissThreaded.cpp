#include "system/omHandleCacheMissThreaded.h"

template < typename TC, typename KEY, typename PTR>
HandleCacheMissThreaded<TC, KEY, PTR>::HandleCacheMissThreaded(TC * tc, KEY key) 
	: mTC(tc)
	, mKey(key)
{

}

template < typename TC, typename KEY, typename PTR >
void HandleCacheMissThreaded<TC, KEY, PTR>::run()
{
	PTR * ret = mTC->HandleCacheMiss(mKey);
	mTC->HandleFetchUpdate(mKey, ret);
}

