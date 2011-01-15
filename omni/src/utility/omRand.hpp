#ifndef OM_RAND_HPP
#define OM_RAND_HPP

#include "zi/omMutex.h"
#include "zi/omUtility.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

class OmRand : private om::singletonBase<OmRand> {
private:
    boost::mt19937 gen_;

public:
    // return rand int in range [min, max]
    static inline int GetRandomInt(const int min, const int max)
    {
		static zi::mutex lock;
		zi::guard g(lock);

        boost::uniform_int<> dist(min, max);
        boost::variate_generator<boost::mt19937&,
            boost::uniform_int<> > rrand(instance().gen_, dist);
        return rrand();
    }

private:
    OmRand()
        : gen_(boost::mt19937(std::time(0)))
    {}

    friend class zi::singleton<OmRand>;
};

#endif
