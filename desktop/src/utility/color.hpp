#pragma once

#include "common/omCommon.h"
#include "project/omProjectGlobals.h"
#include "utility/omRandColorFile.hpp"
#include <QColor>
#include <QPixmap>

namespace om {
namespace utils {

class color {
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

    static QColor OmColorToQColor(const OmColor color){
        return qRgb(color.red, color.green, color.blue);
    }

    static OmColor QColorToOmColor(const QColor color){
        OmColor c = {
        	static_cast<uint8_t>(color.red()),
        	static_cast<uint8_t>(color.green()),
        	static_cast<uint8_t>(color.blue()) };
        return c;
    }

    static QPixmap OmColorAsQPixmap(const OmColor& color)
    {
        return MakeQPixmap(OmColorToQColor(color));
    }

    static QPixmap MakeQPixmap(const QColor& color)
    {
        QPixmap pixm(40, 30);
        pixm.fill(color);
        return pixm;
    }
};

} // namespace utils
} // namespace om