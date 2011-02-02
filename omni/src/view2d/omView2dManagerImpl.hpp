#ifndef OM_VIEW2D_MANAGER_IMPL_HPP
#define OM_VIEW2D_MANAGER_IMPL_HPP

#include "common/om.hpp"
#include "utility/omThreadPool.hpp"
#include "zi/omMutex.h"

class OmView2dManagerImpl {
private:
	OmThreadPool threadPool_;
	std::map<int, std::vector<om::point2d> > ptsInCircle_;

public:
	OmView2dManagerImpl(){
		threadPool_.start();
	}

	~OmView2dManagerImpl(){
		threadPool_.join();
	}

	template <typename T>
	inline void AddTaskBack(const T& task){
		threadPool_.addTaskBack(task);
	}

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
