#ifndef OM_TIMER_H
#define OM_TIMER_H

/*
 *	OmTimer provides a simple cross-platform high-resolution timer for use in
 *  performance testing by wrapping OS-specific timer functions.
 */

#include <zi/time/time.hpp>

class OmTimer {
private:
	zi::wall_timer timer_;

public:
	OmTimer() : timer_()
	{}

	~OmTimer()
	{}

	void start(){
		timer_.reset();
	}

	void restart(){
		timer_.restart();
	}

	double us_elapsed(){
		return timer_.elapsed< zi::in_usecs >();
	}

	double ms_elapsed(){
		return timer_.elapsed< zi::in_msecs >();
	}

	double s_elapsed(){
		return timer_.elapsed< zi::in_msecs >() / 1000.;
	}

	void Print(const std::string& str){
		printf("%s in %.2f secs\n", str.c_str(), s_elapsed());
	}

	void PrintDone(){
		printf("done in %.2f secs\n", s_elapsed());
	}
};

#endif
