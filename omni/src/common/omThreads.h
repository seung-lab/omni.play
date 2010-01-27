
/*
 * Omni Global Graphic Libraries Header File
 *
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#ifndef OM_THREADS_H
#define OM_THREADS_H

#include <pthread.h>





inline bool 
isMutexLocked(pthread_mutex_t *pMutex) {
	if(0 == pthread_mutex_trylock(pMutex)) {
		//got lock, so release
		pthread_mutex_unlock(pMutex);
		return false;
	} else 
		return true;
}



#endif
