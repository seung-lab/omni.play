#pragma once

#include "common/common.h"
#include "boost/format.hpp"

#include <map>
#include <vector>

namespace om {
namespace utility {

class pointsInCircle {
private:
    std::map<int, std::vector<Vector2i> > ptsInCircle_;

    static const int maxBrushSize;
public:
    pointsInCircle()
    {
        for(int i = 1; i <= maxBrushSize; i++) {
            ptsInCircle_[i] = makePtrListInCircle(i);
        }
    }

    const std::vector<Vector2i>& GetPtsInCircle(const int brushDia) const
    {
        if(brushDia <= 0 || brushDia > maxBrushSize)
        {
            throw ArgException(
                str(boost::format("brush size must be between 1 and %") % maxBrushSize));
        }

    return ptsInCircle_.find(brushDia)->second;
    }

private:
    static std::vector<Vector2i> makePtrListInCircle(const int brushDia)
    {
        const int radius   = brushDia / 2;
        const int sqRadius = radius * radius;

        std::vector<Vector2i> pts;
        pts.reserve(brushDia * brushDia);

        for(int i = 0; i < brushDia; ++i)
        {
            const int x = i - radius;

            for(int j = 0; j < brushDia; ++j)
            {
                const int y = j - radius;

                if( x * x + y * y <= sqRadius )
                {
                    Vector2i p(x, y);
                    pts.push_back(p);

                    // std::cout << "adding pt: " << x << ", " << y << "\n";
                }
            }
        }

        return pts;
    }
};

const int pointsInCircle::maxBrushSize = 20;

} // namespace utility
} // namespace om
