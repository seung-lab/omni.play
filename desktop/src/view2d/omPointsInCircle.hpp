#pragma once

#include "zi/omMutex.h"

#include <map>
#include <vector>

class OmPointsInCircle {
private:
    std::map<int, std::vector<Vector2i> > ptsInCircle_;

    zi::mutex lock_;

public:
    const std::vector<Vector2i>& GetPtsInCircle(const int brushDia)
    {
        zi::guard g(lock_);

        if(!ptsInCircle_.count(brushDia)){
            return ptsInCircle_[brushDia] = makePtrListInCircle(brushDia);
        }
        return ptsInCircle_[brushDia];
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
                    Vector2i p(x,y);
                    pts.push_back(p);

                    // std::cout << "adding pt: " << x << ", " << y << "\n";
                }
            }
        }

        return pts;
    }
};

