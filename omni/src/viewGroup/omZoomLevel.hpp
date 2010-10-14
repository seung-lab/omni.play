#ifndef OM_ZOOM_LEVEL_HPP
#define OM_ZOOM_LEVEL_HPP

class OmZoomLevel {
private:
	bool valid;
	int mipLevel_;
	float zoomFactor_;

public:
	OmZoomLevel()
		: valid(false)
		, mipLevel_(0)
		, zoomFactor_(0.6)
	{}

	void Update(const int defaultMipLevel){
		if(!valid){
			Reset(defaultMipLevel);
		}
	}

	void Reset(const int defaultMipLevel){
		mipLevel_ = defaultMipLevel;
		zoomFactor_ = 1;
		valid = true;
	}

	float GetZoomScale() const {
		return zoomFactor_;
	}

	int GetMipLevel() const {
		return mipLevel_;
	}

	void MouseWheelZoom(const int numSteps, const bool isLevelLocked,
						const int maxMipLevel)
	{
		if (numSteps >= 0){
			mouseWheelZoomIn(numSteps, isLevelLocked);
		} else{
			mouseWheelZoomOut(numSteps, isLevelLocked, maxMipLevel);
		}
	}

private:
	void mouseWheelZoomOut(const int numSteps, const bool isLevelLocked,
						   const int maxMipLevel)
	{
		if(!isLevelLocked && zoomFactor_ <= 0.6 && mipLevel_ < maxMipLevel){
			// move to next mip level
			mipLevel_ += 1;
			zoomFactor_ = 1.0;
			return;
		}

		zoomFactor_ += numSteps / 10.0; // numSteps is negative!
		if( zoomFactor_ < 0.1 ){
			zoomFactor_ = 0.1;
		}
	}

	void mouseWheelZoomIn(const int numSteps, const bool isLevelLocked)
	{
		if(!isLevelLocked && zoomFactor_ >= 1 && mipLevel_ > 0){
			//move to previous mip level
			mipLevel_ -= 1;
			zoomFactor_ = 0.6;
			return;
		}

		zoomFactor_ += numSteps / 10.0;
	}

};

#endif
