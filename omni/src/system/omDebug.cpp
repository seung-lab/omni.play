
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// TODO Convert to "flag-oriented" switches ---> get rid of #defs
// ----> to create command line debugging. 

//***********************************************************************
//****  SET YOUR DEF FLAGS HERE                     
//***********************************************************************

#define OM_CACHE           1        // Testing of the Cache
#define OM_TESTRUN         0        // Gen One's Old Debugs
#define OM_MUTEX           0        // pthread_mutexes - all mutexes noted
#define OM_MESHER          1 
//*********************************************************************** 
//***********************************************************************




// Your general debug function . . . it prints
void debug(const char *category,const char *format, ...)
{
        va_list args;
        va_start(args, format);
	
	if (strcmp(category,"cache")){
#if OM_CACHE
                vfprintf(stdout,format,args);		        
#endif
	}

	  if (strcmp(category,"testrun")){
#if OM_TESTRUN
	    vfprintf(stdout,format,args);
#endif
	}

	if (strcmp(category,"mesher")){
#if OM_MESHER
                vfprintf(stdout,format,args);		        
#endif
	}
	va_end(args);


}



	  
#if OM_MUTEX
void omThreadedHelperLock(pthread_mutex_t* x, int line, const char * fun)
{
        fprintf(stdout,"locking enter: %p (line:fun) %i:%s \n",x,line,fun);
	pthread_mutex_lock (x);
        fprintf(stdout,"locking exit : %p (line:fun) %i:%s \n",x,line,fun);

}

void omThreadedHelperUnlock(pthread_mutex_t*  x, int line, const char * fun)
{
        fprintf(stdout,"unlocking enter: %p (line:fun) %i:%s \n",x,line,fun);
	pthread_mutex_unlock (x);
        fprintf(stdout,"unlocking exit : %p (line:fun) %i:%s \n",x,line,fun);

}
#else
void omThreadedHelperLock(pthread_mutex_t* x, int line, const char * fun)
{
 
	pthread_mutex_lock (x);


}

void omThreadedHelperUnlock(pthread_mutex_t*  x, int line, const char * fun)
{

	pthread_mutex_unlock (x);

}

#endif

