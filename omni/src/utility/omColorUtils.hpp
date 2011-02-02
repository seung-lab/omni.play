#ifndef OM_COLOR_UTILS_HPP
#define OM_COLOR_UTILS_HPP

#include "common/omCommon.h"
#include "project/omProjectGlobals.h"
#include "utility/omRandColorFile.hpp"

class OmColorUtils {
private:
    enum ColorIndexInternal {RED, GREEN, BLUE};

public:

    static inline OmColor GetRandomColor(){
		return OmProject::Globals().RandColorFile().GetRandomColor();
	}

    static inline OmColor GetRandomColor(const OmColor old)
    {
        ColorIndexInternal olc;
        ColorIndexInternal nsc;
        OmColor ret = GetRandomColor();

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
};

#endif
