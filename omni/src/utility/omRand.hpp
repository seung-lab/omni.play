#ifndef OM_RAND_HPP
#define OM_RAND_HPP

#include "zi/omUtility.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

class OmRand : private om::singletonBase<OmRand> {
public:
    // return rand int in range [min, max]
    static inline int GetRandomInt(const int min, const int max)
    {
        boost::uniform_int<> dist(min, max);
        boost::variate_generator<boost::mt19937&,
            boost::uniform_int<> > rrand(instance().gen_, dist);
        return rrand();
    }

    static inline OmColor GetRandomColor()
    {
        //OmColor colorInt = { GetRandomInt(1,127),
        //GetRandomInt(1,127),
        //GetRandomInt(1,127) };
        //return colorInt;
        return OmRand::getRandomColorImpl();
    }

private:

    static OmColor getRandomColorImpl()
    {
        int x, y, z;
        int v = 0;

        static const int min_variance = 120;

        while ( v < min_variance )
        {
            x = GetRandomInt(1,127);
            y = GetRandomInt(1,127);
            z = GetRandomInt(1,127);

            int avg  = ( x + y + z ) / 3;
            int avg2 = ( x*x + y*y + z*z ) / 3;

            v = avg2 - avg*avg;
        }

        OmColor ret = { x, y, z };

        return ret;
    }

    OmRand()
        : gen_(boost::mt19937(std::time(0)))
    {}

    boost::mt19937 gen_;

    friend class zi::singleton<OmRand>;
};

#endif
