#ifndef _OM_DEBUG_H_
#define _OM_DEBUG_H_

void debug(const char *category, const char *format, ...);

#define pthread_mutex_lock(x) debug("mutex","locking enter: %p (line:fun) %i:%s \n",x,__LINE__, __FUNCTION__);\
        pthread_mutex_lock(x);                                             \
        debug("mutex","locking exit: %p (line:fun) %i:%s \n",x,__LINE__, __FUNCTION__);

#define pthread_mutex_unlock(x) debug("mutex","unlocking enter: %p (line:fun) %i:%s \n",x,__LINE__, __FUNCTION__);\
        pthread_mutex_unlock(x);\
        debug("mutex","unlocking exit: %p (line:fun) %i:%s \n",x,__LINE__, __FUNCTION__);
#endif
