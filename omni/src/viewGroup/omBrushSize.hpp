#ifndef OM_BRUSH_SIZE_HPP
#define OM_BRUSH_SIZE_HPP

#include "view2d/omPointsInCircle.hpp"

class OmBrushSize{
private:
	static const int defaultBrushDia = 8;

	OmPointsInCircle circlePts_;
	std::vector<om::point2d> ptsInBrush_;

public:
	OmBrushSize(){
		setDiameter(defaultBrushDia);
	}

	inline int Diameter() const {
		return diameter_;
	}

	void SetDiameter(const int size){
		setDiameter(size);
	}

	void IncreaseSize(){
		setDiameter(getNextSizeUp());
	}

	void DecreaseSize(){
		setDiameter(getNextSizeDown());
	}

	const std::vector<om::point2d>& GetPtsInCircle() const {
		return ptsInBrush_;
	}

private:
	int diameter_;

	void setDiameter(const int diameter)
	{
		diameter_ = diameter;
		setPtsInCircle();
	}

	void setPtsInCircle(){
		ptsInBrush_ = circlePts_.GetPtsInCircle(diameter_);
	}

	int getNextSizeUp(){
		switch(diameter_){
		case 1:
			return 2;
		case 2:
			return 8;
		case 8:
			return 16;
		case 16:
			return 32;
		case 32:
		case 64:
		default:
			return 64;
		}
	}

	int getNextSizeDown(){
		switch(diameter_){
		case 1:
		case 2:
			return 1;
		case 8:
			return 2;
		case 16:
			return 8;
		case 32:
			return 16;
		case 64:
			return 32;
		default:
			return 1;
		}
	}
};

#endif
