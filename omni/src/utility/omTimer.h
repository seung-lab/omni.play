#ifndef OM_TIMER_H
#define OM_TIMER_H

/*
 *	OmTimer provides a simple cross-platform high-resolution timer for use in performance 
 *	testing by wrapping OS-specific timer functions.	
 */

#include "common/omStd.h"

#ifdef WIN32
#include <windows.h> //For Windows
#else
#include <sys/time.h> //For Unix/Linux/Mac OS
#endif

class OmTimer {

public:
	OmTimer();
	~OmTimer();

	void start();
	void stop();

	double us_elapsed();
	double ms_elapsed();
	double s_elapsed();

private:
	
	#ifdef WIN32
	LARGE_INTEGER start_time, stop_time;
	LARGE_INTEGER freq;
	#else
	timeval start_time, stop_time;
	#endif

	bool running;
};

#endif
