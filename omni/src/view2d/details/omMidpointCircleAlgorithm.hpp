#ifndef OM_MIDPOINT_CIRCLE_ALGORITHM_HPP
#define OM_MIDPOINT_CIRCLE_ALGORITHM_HPP

typedef struct {
	int x;
	int y;
} Om2dPoint;

bool operator <(const Om2dPoint& a, const Om2dPoint& b)
{
	if(a.x != b.x){
		return a.x < b.x;
	}

	return a.y < b.y;
}

bool operator ==(const Om2dPoint& a, const Om2dPoint& b)
{
	return a.x == b.x && a.y == b.y;
}

class OmMidpointCircleAlgorithm {
private:
	std::set<Om2dPoint> pointsOnCircumfrence_;

public:

	std::set<Om2dPoint> GetPointsOnCircumfrence(const int r)
	{
		computePixelsOnCircumfrence(r,r,r);
		return pointsOnCircumfrence_;
	}

private:
	void addPoint(const int x, const int y)
	{
		Om2dPoint p = { x, y };
		pointsOnCircumfrence_.insert(p);
	}

	// from http://en.wikipedia.org/wiki/Talk:Midpoint_circle_algorithm
	void computePixelsOnCircumfrence(const int x0,
									 const int y0,
									 const int r){
		int u, v;
		bool diagonal = false;

        // start from a know pixel (u,v)=(r,0) on the x axis,
		//  compute only the first octant (u > v)
		for (u = r, v = 0; u > v;) {
			addPoint(x0 + u, y0 + v);
			addPoint(x0 + v, y0 + u);
			addPoint(x0 - u, y0 + v);
			addPoint(x0 - v, y0 + u);
			addPoint(x0 + u, y0 - v);
			addPoint(x0 + v, y0 - u);
			addPoint(x0 - u, y0 - v);
			addPoint(x0 - v, y0 - u);
			v++; // next pixel is above (in the first octant)

			/* if the pixel above is at distance greater than 1/2+r
			 * from the origin, we could test if (v^2 + u^2 >= (1/2+r)^2),
			 * but the following is equivalent with integers:
			 */
			diagonal = v*v + u*u - (1 + r)*r > 0;
			if(diagonal){
				// choose the diagonal pixel instead
				// (to the right in the first octant)
				u--;
			}
		}

		if (u == v && diagonal) { // last pixel if it's on the 45deg diagonal
			addPoint(x0 + u, y0 + u);
			addPoint(x0 - u, y0 + u);
			addPoint(x0 + u, y0 - u);
			addPoint(x0 - u, y0 - u);
		}
	}
};

#endif
