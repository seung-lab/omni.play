#ifndef OM_POINTS_IN_CIRCLE_HPP
#define OM_POINTS_IN_CIRCLE_HPP

#include "common/om.hpp"
#include "zi/omMutex.h"

class OmPointsInCircle {
private:
	std::map<int, std::vector<om::point2d> > ptsInCircle_;

public:
	const std::vector<om::point2d>& GetPtsInCircle(const int brushDia)
	{
		static zi::mutex lock;
		zi::guard g(lock);

		if(!ptsInCircle_.count(brushDia)){
			return ptsInCircle_[brushDia] =	makePtrListInCircle(brushDia);
		}
		return ptsInCircle_[brushDia];
	}

private:
	static std::vector<om::point2d> makePtrListInCircle(const int brushDia)
	{
        const int radius   = brushDia / 2;
        const int sqRadius = radius * radius;

		std::vector<om::point2d> pts;
		pts.reserve(brushDia * brushDia);

        for(int i = 0; i < brushDia; ++i)
		{
			const int x = i - radius;

            for(int j = 0; j < brushDia; ++j)
            {
                const int y = j - radius;

                if( x * x + y * y <= sqRadius )
                {
					om::point2d p = {x,y};
					pts.push_back(p);
                }
            }
        }

		return pts;
	}
};

#endif
