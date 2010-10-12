#ifndef OM_ZOOM_LEVEL_HPP
#define OM_ZOOM_LEVEL_HPP

#include "volume/omMipVolume.h"
#include "system/omEvents.h"

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

/*
	float getZoomScale() const {
		return zoomLevel_->GetZoomLevel().y / 10.0;
	}
	int getMipLevel() const {
		return zoomLevel_->GetZoomLevel().x;
	}
*/
	Vector2i GetZoomLevel() const {
		return Vector2i(mipLevel_, zoomFactor_);
	}

	void SetZoomLevel(const Vector2i& zoom)
	{
		mipLevel_ = zoom.x;
		zoomFactor_ = zoom.y;
		OmEvents::ViewPosChanged();
	}
};

#endif
