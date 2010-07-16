#ifndef OM_HANDLE_CACHE_MISS_THREADED_H
#define OM_HANDLE_CACHE_MISS_THREADED_H

#include <QRunnable>

template < typename TC, typename KEY, typename PTR >
class HandleCacheMissThreaded : public QRunnable
{
 public:
        HandleCacheMissThreaded(TC * tc);
        void run();

 private:
	TC * const mTC;
	KEY mKey;

	void HandleFetchUpdate(PTR * fetch_value);
};

#endif
