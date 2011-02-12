#ifndef OM_BRUSH_SIZE_HPP
#define OM_BRUSH_SIZE_HPP

class OmBrushSize{
public:
	OmBrushSize()
		: diameter_(8) {}

	int Diameter(){ return diameter_; }

	void SetDiameter(const int size){
		diameter_ = size;
	}

	void IncreaseSize(){
		diameter_ = getNextSizeUp();
	}

	void DecreaseSize(){
		diameter_ = getNextSizeDown();
	}

private:
	int diameter_;

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
