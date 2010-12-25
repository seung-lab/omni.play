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
        const OmColor ret = getRandomColorImpl();
        return ret;
    }

private:
    enum ColorIndexInternal {RED, GREEN, BLUE};

public:
    static inline OmColor GetRandomColor(OmColor old)
    {
        ColorIndexInternal olc;
        ColorIndexInternal nsc;
        OmColor ret = getRandomColorImpl();

        if(old.red > old.blue && old.red > old.green) {
			olc = RED;
        } else if(old.blue > old.red && old.blue > old.green) {
			olc = BLUE;
        } else if(old.green > old.red && old.green > old.blue) {
			olc = GREEN;
        }

        if(ret.red < ret.blue && ret.red < ret.green) {
			nsc = RED;
        } else if(ret.blue < ret.red && ret.blue < ret.green) {
			nsc = BLUE;
        } else if(ret.green < ret.red && ret.green < ret.blue) {
			nsc = GREEN;
        }

		if((RED == olc && BLUE == nsc) || (BLUE == olc && RED == nsc)) {
			std::swap(ret.red, ret.blue);
		}

		if((RED == olc && GREEN == nsc) || (GREEN == olc && RED == nsc)){
			std::swap(ret.red, ret.green);
		}

		if((BLUE == olc && GREEN == nsc) || (GREEN == olc && BLUE == nsc)){
			std::swap(ret.blue, ret.green);
		}

        return ret;
    }

private:

    static OmColor getRandomColorImpl()
    {
        int r, g, b;
        int v = 0;

        static const int min_variance = 120;

        while ( v < min_variance )
        {
            r = GetRandomInt(1,127);
            g = GetRandomInt(1,127);
            b = GetRandomInt(1,127);

            int avg  = ( r + g + b ) / 3;
            int avg2 = ( r*r + g*g + b*b ) / 3;

            v = avg2 - avg*avg;
        }

        OmColor ret = { r, g, b };

        return ret;
    }

    OmRand()
        : gen_(boost::mt19937(std::time(0)))
    {}

    boost::mt19937 gen_;

    friend class zi::singleton<OmRand>;
};

#endif
