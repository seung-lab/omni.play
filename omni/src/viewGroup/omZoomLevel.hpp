#ifndef OM_ZOOM_LEVEL_HPP
#define OM_ZOOM_LEVEL_HPP

#include "volume/omMipVolume.h"

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

	void Update(OmMipVolume* vol){
		if(!valid){
			// set defaults
			mipLevel_ = vol->GetRootMipLevel();
			zoomFactor_ = 1;
			valid = true;
		}
	}

	float GetZoomScale() const {
		return zoomFactor_;
	}

	int GetMipLevel() const {
		return mipLevel_;
	}

	void MouseWheelZoom(const int numSteps, const bool isLevelLocked,
						OmMipVolume* vol)
	{
		if (numSteps >= 0){
			mouseWheelZoomIn(numSteps, isLevelLocked);
		} else{
			const int maxMipLevel = vol->GetRootMipLevel();
			mouseWheelZoomOut(numSteps, isLevelLocked, maxMipLevel);
		}
	}

private:
	void mouseWheelZoomOut(const int numSteps, const bool isLevelLocked,
						   const int maxMipLevel)
	{
		if (!isLevelLocked && zoomFactor_ <= 0.6 && mipLevel_ < maxMipLevel){
			mipLevel_ += 1; // need to move to next mip level
			zoomFactor_ = 1.0;

		} else if (zoomFactor_ > 0.1) {
			int zoom = zoomFactor_ * 10 - (1 * (-1 * numSteps));
			if (zoom < 1) {
				zoom = 1;
			}
			zoomFactor_ = zoom / 10.0;
		}
	}

	void mouseWheelZoomIn(const int numSteps, const bool isLevelLocked)
	{
		if(!isLevelLocked && zoomFactor_ >= 1 && mipLevel_ > 0){
			mipLevel_ -= 1; //move to previous mip level
			zoomFactor_ = 0.6;

		} else{
			zoomFactor_ = ceil(zoomFactor_ * 10.0 + (1 * numSteps)) / 10.0;
		}
	}

};

#endif
