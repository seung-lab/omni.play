#ifndef _OM_DEBUG_H_
#define _OM_DEBUG_H_

#define pthread_mutex_lock(x) omThreadedHelperLock(x, __LINE__, __FUNCTION__)
#define pthread_mutex_unlock(x) omThreadedHelperUnlock(x, __LINE__, __FUNCTION__)


void omThreadedHelperLock(pthread_mutex_t *x, int line, const char * fun);
void omThreadedHelperUnlock(pthread_mutex_t *x, int line, const char * fun);


void debug(const char *category,const char *format, ...);



#endif


