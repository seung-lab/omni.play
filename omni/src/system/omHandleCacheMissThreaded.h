#ifndef __HANDLE_CACHE_MISS_THREADED_H__
#define __HANDLE_CACHE_MISS_THREADED_H__

#include <QThread>

template < typename TC, typename KEY, typename PTR >
class HandleCacheMissThreaded : public QThread
{
public:
        HandleCacheMissThreaded(TC * tc, KEY key);
        void run();
private:
	TC * const mTC;
	KEY const mKey;

};

#endif
