#ifndef OM_COLOR_UTILS_HPP
#define OM_COLOR_UTILS_HPP

#include "common/omCommon.h"
#include "utility/omRand.hpp"

class OmColorUtils {
private:
    enum ColorIndexInternal {RED, GREEN, BLUE};

public:
    static inline OmColor GetRandomColor()
	{
		static bool builtColorTable = false;
		static std::vector<OmColor> colorTable;
		static zi::mutex lock;

		{
			zi::guard g(lock);
			if(!builtColorTable){
				buildColorTable(colorTable);
				builtColorTable = true;
			}
		}

		const int index = OmRand::GetRandomInt(0, colorTable.size()-1);
        return colorTable[index];
    }

private:
	static void buildColorTable(std::vector<OmColor>& colorTable)
	{
		colorTable.clear();
		colorTable.reserve(1952449); // table will have 1,952,448 entries

		static const int min_variance = 120;

		for(int r = 0; r < 128; ++r){
			for(int g = 0; g < 128; ++g){
				for(int b = 0; b < 128; ++b){

					const int avg  = ( r + g + b ) / 3;
					const int avg2 = ( r*r + g*g + b*b ) / 3;
					const int v = avg2 - avg*avg;

					if(v >= min_variance){
						const OmColor color = {r, g, b};
						colorTable.push_back(color);
					}
				}
			}
		}
	}

public:
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
