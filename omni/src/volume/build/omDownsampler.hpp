#ifndef OM_DOWNSAMPLER_HPP
#define OM_DOWNSAMPLER_HPP

#include "volume/omMipVolume.h"

template <typename T>
class OmDownsampler {
private:
	OmMipVolume *const vol_;
	std::vector<T*> mips_;

	const Vector3<uint64_t> srcDims_;
	const Vector3<uint64_t> chunkDims_;
	const uint64_t slabSize_;
	const uint64_t rowSize_;
	const uint64_t chunkSize_;

public:
	OmDownsampler(OmMipVolume* vol)
		: vol_(vol)
		, srcDims_(vol->getDimsRoundedToNearestChunk(0))
		, chunkDims_(vol_->GetChunkDimensions())
		, slabSize_( srcDims_.x   * srcDims_.y   * chunkDims_.z)
		, rowSize_(  srcDims_.x   * chunkDims_.y * chunkDims_.z)
		, chunkSize_(chunkDims_.x * chunkDims_.y * chunkDims_.z)
	{
		// get ptrs to each MIP volume level
		mips_.resize(vol_->GetRootMipLevel() + 1);
		for(int i=0; i <= vol_->GetRootMipLevel(); ++i){
			mips_[i] = boost::get<T*>(vol_->getVolData()->GetVolPtr(i));
			assert(mips_[i]);
		}


//		const Vector3<int64_t> chunkPos = coord.Coordinate; // bottom left corner
//		const int64_t offset =
//			slabSize*chunkPos.z + rowSize*chunkPos.y + chunkSize*chunkPos.x;
	}

	void DownsampleChooseOneVoxelOfNine()
	{
		for(uint64_t sz=0; sz < srcDims_.z; sz+=2){
			for(uint64_t sy=0; sy < srcDims_.y; sy+=2){
				for(uint64_t sx=0; sx < srcDims_.x; sx+=2){



				}
			}
		}
	}

};

#endif
