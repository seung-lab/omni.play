#ifndef OM_DOWNSAMPLER_HPP
#define OM_DOWNSAMPLER_HPP

#include "volume/omMipVolume.h"

template <typename T>
class OmDownsampler {
private:
	OmMipVolume* vol_;
	std::vector<T*> mips_;
	const Vector3<uint64_t> src_dims_;

public:
	OmDownsampler(OmMipVolume* vol)
		: vol_(vol)
		, src_dims_(vol->getDimsRoundedToNearestChunk(0))
	{
		// get ptrs to each MIP volume level
		mips_.resize(vol_->GetRootMipLevel() + 1);
		for(int i=0; i <= vol_->GetRootMipLevel(); ++i){
			mips_[i] = boost::get<T*>(vol_->getVolData()->GetVolPtr(i));
			assert(mips_[i]);
		}
	}

	void DownsampleChooseOne()
	{
		for(uint64_t sz=0; sz < src_dims_.z; sz+=2){
			for(uint64_t sy=0; sy < src_dims_.y; sy+=2){
				for(uint64_t sx=0; sx < src_dims_.x; sx+=2){



				}
			}
		}
	}

};

#endif
