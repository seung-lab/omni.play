#include "omTimer.h"

OmTimer::OmTimer()
{
	#ifdef WIN32
	QueryPerformanceFrequency(&freq);
	start_time.QuadPart = 0;
	stop_time.QuadPart = 0;
	#else
	start_time.tv_sec = 0;
	start_time.tv_usec = 0;
	stop_time.tv_sec = 0;
	stop_time.tv_usec = 0;
	#endif

	running = false;
}

OmTimer::~OmTimer(){}

void OmTimer::start()
{
	#ifdef WIN32
	QueryPerformanceCounter(&start_time);
	#else
	gettimeofday(&start_time,NULL);
	#endif

	running = true;
}

void OmTimer::stop()
{
	if (running){
		#ifdef WIN32
		QueryPerformanceCounter(&stop_time);
		#else
		gettimeofday(&stop_time,NULL);
		#endif
	}

	running = false;
}

double OmTimer::us_elapsed()
{
	if (running){
		#ifdef WIN32
		QueryPerformanceCounter(&stop_time);
		#else
		gettimeofday(&stop_time,NULL);
		#endif
	}

	#ifdef WIN32
	return (stop_time.QuadPart - start_time.QuadPart) * 1000000.0 / freq.QuadPart;
	#else
	return (stop_time.tv_sec - start_time.tv_sec) * 1000000.0 + (stop_time.tv_usec - start_time.tv_usec);
	#endif
}

double OmTimer::ms_elapsed()
{
	return us_elapsed() / 1000.0;
}

double OmTimer::s_elapsed()
{
	return us_elapsed() / 1000000.0;
}
