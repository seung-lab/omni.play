#ifndef OM_RAND_HPP
#define OM_RAND_HPP

#include "zi/omUtility.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

class OmRand : public zi::enable_singleton_of_this<OmRand> {
public:
	// return rand int in range [min, max]
	static int GetRandomInt(const int min, const int max)
	{
		boost::uniform_int<> dist(min, max);
		boost::variate_generator<boost::mt19937&,
			boost::uniform_int<> > rrand(instance().gen_, dist);
		return rrand();
	}

	static OmColor GetRandomColor()
	{
		OmColor colorInt = {GetRandomInt(1,127),
							GetRandomInt(1,127),
							GetRandomInt(1,127)};
		return colorInt;
	}

private:
	OmRand()
		: gen_(boost::mt19937(std::time(0)))
	{}

	boost::mt19937 gen_;

 	friend class zi::singleton<OmRand>;
};

#endif
