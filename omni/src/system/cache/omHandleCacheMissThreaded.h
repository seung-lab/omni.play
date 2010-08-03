#ifndef OM_HANDLE_CACHE_MISS_THREADED_H
#define OM_HANDLE_CACHE_MISS_THREADED_H

#include <zi/threads>

template < typename TC, typename KEY, typename PTR >
class HandleCacheMissThreaded : public zi::Runnable
{
 public:
        HandleCacheMissThreaded(TC * tc, const KEY & key);
        void run();

 private:
	TC * const mTC;
	KEY mKey;

	void HandleFetchUpdate(PTR fetch_value);
};

#endif
