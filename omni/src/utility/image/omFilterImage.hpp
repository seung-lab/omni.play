#ifndef OM_IMAGE_FILTER_HPP
#define OM_IMAGE_FILTER_HPP

#include "utility/image/omImage.hpp"
#include "zi/omUtility.h"

class OmImageFilter : private om::singletonBase<OmImageFilter>{
public:
	static boost::shared_ptr<uint8_t>
	FilterChannel(OmImage<uint8_t, 2>& slice)
	{
		static const uint8_t absMax = 255;
		static const int32_t defaultBrightness = 0;
		static const float defaultContrast = 1;
		static const double defaultGamma = 1.0;

		if(defaultBrightness != instance().brightnessShift_){
			slice.Brightness(absMax, instance().brightnessShift_);
		}

		if(defaultContrast != instance().contrastValue_){
			slice.Contrast(absMax, instance().contrastValue_);
		}

		//TODO: use epsilon for double comparison...
		if(defaultGamma != instance().gamma_){
			slice.Gamma(instance().gamma_);
		}

		return slice.getMallocCopyOfData();
	}

	static int32_t GetBrightnessShift(){
		return instance().brightnessShift_;
	}
	static void SetBrightnessShift(const int32_t shift){
		instance().brightnessShift_ = shift;
	}

	static uint8_t GetContrastValue(){
		return instance().contrastValue_;
	}
	static void SetContrastValue(const float contrast){
		instance().contrastValue_ = contrast;
	}

	static double GetGamma(){
		return instance().gamma_;
	}
	static void SetGamma(const double gamma){
		instance().gamma_ = gamma;
	}

private:
	int32_t brightnessShift_;
	float contrastValue_;
	double gamma_;

	OmImageFilter()
		: brightnessShift_(0)
		, contrastValue_(1)
		, gamma_(1.0)
	{}

	friend class zi::singleton<OmImageFilter>;
};

#endif
