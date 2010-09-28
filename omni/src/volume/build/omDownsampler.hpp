#ifndef OM_DOWNSAMPLER_HPP
#define OM_DOWNSAMPLER_HPP

#include "volume/omMipVolume.h"

template <typename T>
class OmDownsampler {
private:
	OmMipVolume* vol_;

public:
	OmDownsampler(OmMipVolume* vol)
		: vol_(vol)
	{}

	void DownsampleChooseOne()
	{
		const Vector3<uint64_t> src_dims = vol_->getDimsRoundedToNearestChunk(0);
		std::vector<T*> mips(vol_->GetRootMipLevel() + 1);
		for(int i=0; i <= vol_->GetRootMipLevel(); ++i){
			mips[i] = boost::get<T*>(vol_->getVolData()->GetVolPtr(i));
			assert(mips[i]);
		}

		for(uint64_t sz=0; sz < src_dims.z; sz+=2){
			for(uint64_t sy=0; sy < src_dims.y; sy+=2){
				for(uint64_t sx=0; sx < src_dims.x; sx+=2){

				}
			}
		}

	}

};

#endif
