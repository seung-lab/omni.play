#ifndef OM_ZOOM_LEVEL_HPP
#define OM_ZOOM_LEVEL_HPP

#include "volume/omMipVolume.h"

class OmZoomLevel {
private:
	bool valid;
	int mipLevel_;
	int zoomFactor_;

public:
	OmZoomLevel()
		: valid(false)
		, mipLevel_(0)
		, zoomFactor_(6)
	{}

	void Update(OmMipVolume* vol){
		if(!valid){
			// set defaults
			mipLevel_ = vol->GetRootMipLevel();
			zoomFactor_ = 10;
			valid = true;
		}
	}

	float GetZoomScale() const {
		return zoomFactor_ / 10.0;
	}

	int GetMipLevel() const {
		return mipLevel_;
	}

	void MouseWheelZoom(const int numSteps, const bool isLevelLocked,
						OmMipVolume* vol)
	{
		const int curMipLevel = mipLevel_;
		const float curZoom = GetZoomScale();
		int mipLevel;
		int zoomFactor;

		if (numSteps >= 0) { // ZOOMING IN
			if(!isLevelLocked && curZoom >= 1 && curMipLevel > 0){
				//move to previous mip level
				mipLevel = curMipLevel - 1;
				zoomFactor = 6;
			} else{
				mipLevel = curMipLevel;
				zoomFactor = ceil(curZoom * 10.0 + (1 * numSteps));
			}
		} else { // ZOOMING OUT
			if (!isLevelLocked && curZoom <= 0.6 &&
			    curMipLevel < vol->GetRootMipLevel()){
				// need to move to next mip level
				mipLevel = curMipLevel + 1;
				zoomFactor = 10;
			} else if (curZoom > 0.1) {
				int zoom = curZoom * 10 - (1 * (-1 * numSteps));
				if (zoom < 1) {
					zoom = 1;
				}
				mipLevel = curMipLevel;
				zoomFactor = zoom;
			}else{
				return;
			}
		}

		mipLevel_ = mipLevel;
		zoomFactor_ = zoomFactor;
	}

};

#endif
