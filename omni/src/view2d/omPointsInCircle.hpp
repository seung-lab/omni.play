#ifndef OM_POINTS_IN_CIRCLE_HPP
#define OM_POINTS_IN_CIRCLE_HPP

#include "view2d/details/omMidpointCircleAlgorithm.hpp"

class OmPointsInCircle {
private:
	boost::unordered_map<int, std::vector<Om2dPoint> > pointLists_;

public:
	const std::vector<Om2dPoint>& GetPointsInCircleCached(const int r)
	{
		if(!pointLists_.count(r)){
			return pointLists_[r] = getPoints(r);
		}
		return pointLists_[r];
	}

	std::set<Om2dPoint> GetPointsInCircleNonCached(const int r)
	{
		return getPointsSet(r);
	}

private:

	// fill using horizontal scan lines
	std::set<Om2dPoint> getPointsSet(const int r)
	{
		OmMidpointCircleAlgorithm mca;
		const std::set<Om2dPoint> pointsOnCircumfrence =
			mca.GetPointsOnCircumfrence(r);

		std::set<Om2dPoint> points;

		FOR_EACH(p, pointsOnCircumfrence){
			if(p->x > r){
				break;
			}

			for(int i = p->x; i <= 2*r-p->x; ++i){
				Om2dPoint fp = { i, p->y };
				points.insert(fp);
			}
		}

		return points;
	}

	std::vector<Om2dPoint> getPoints(const int r)
	{
		std::set<Om2dPoint> points = getPointsSet(r);
		std::vector<Om2dPoint> ret(points.size());
		std::copy(points.begin(), points.end(), ret.begin());
		return ret;
	}

};

#endif
