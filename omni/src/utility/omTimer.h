#ifndef OM_TIMER_H
#define OM_TIMER_H

/*
 *	OmTimer provides a simple cross-platform high-resolution timer for use in performance
 *	testing by wrapping OS-specific timer functions.
 */

/*
 * take a look at <zi2/time/now.hpp> to see how to get system time
 * in windows
 */

#include <zi2/time/time.hpp>

class OmTimer
{
private:
	zi::wall_timer timer_  ;

public:
	OmTimer() : timer_()
	{
	}

	~OmTimer()
	{
	}

	void start()
	{
		timer_.reset();
	}

	void restart()
	{
		timer_.restart();
	}

	double us_elapsed()
	{
		return timer_.us_elapsed< double >();
	}

	double ms_elapsed()
	{
		return timer_.ms_elapsed< double >();
	}

	double s_elapsed()
	{
		return timer_.elapsed< double >();
	}
};

#endif
