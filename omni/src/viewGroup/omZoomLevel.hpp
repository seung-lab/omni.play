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

	void SetZoomLevel(const int mipLevel, const int zoomFactor)
	{
		mipLevel_ = mipLevel;
		zoomFactor_ = zoomFactor;
	}
};

#endif
