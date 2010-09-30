#ifndef OM_DOWNSAMPLER_HPP
#define OM_DOWNSAMPLER_HPP

#include "volume/omMipVolume.h"

template <typename T>
class OmDownsampler {
private:
	OmMipVolume *const vol_;
	std::vector<T*> mips_;
	std::vector<uint64_t> mipsFactor_;
	const int maxMipLevel_;

	const Vector3<uint64_t> srcDims_;
	const Vector3<uint64_t> chunkDims_;
	const uint64_t tileSize_;
	const uint64_t slabSize_;
	const uint64_t sliceSize_;
	const uint64_t rowSize_;
	const uint64_t chunkSize_;
	const uint64_t totalVoxels_;

public:
	OmDownsampler(OmMipVolume* vol)
		: vol_(vol)
		, maxMipLevel_(vol_->GetRootMipLevel())
		, srcDims_(vol->getDimsRoundedToNearestChunk(0))
		, chunkDims_(vol_->GetChunkDimensions())
		, tileSize_(chunkDims_.x * chunkDims_.y)
		, slabSize_( srcDims_.x   * srcDims_.y   * chunkDims_.z)
		, sliceSize_(srcDims_.x   * srcDims_.y)
		, rowSize_(  srcDims_.x   * chunkDims_.y * chunkDims_.z)
		, chunkSize_(chunkDims_.x * chunkDims_.y * chunkDims_.z)
		, totalVoxels_(srcDims_.x * srcDims_.y   * srcDims_.z)
	{
		// get ptrs to each MIP volume level
		mips_.resize(maxMipLevel_ + 1);
		for(int i=0; i <= maxMipLevel_; ++i){
			mips_[i] = boost::get<T*>(vol_->getVolData()->GetVolPtr(i));
			assert(mips_[i]);
		}

		mipsFactor_.resize(maxMipLevel_ + 1);
		for(int i=0; i <= maxMipLevel_; ++i){
			mipsFactor_[i] = OMPOW(2, i);
		}
	}

/*
	uint64_t getOffsetIntoVolume(uint64_t x, uint64_t y, uint64_t z, int level)
	{
		for( int level = 1; level <= vol_->GetRootMipLevel(); ++level){
			const uint64_t numToSkip = OMPOW(2, level);
		const uint64_t

		const Vector3i chunkPos = (ROUNDDOWN(sx,128),
								   ROUNDDOWN(sy,128),
								   ROUNDDOWN(sz,128));
		uint64_t offset =
			slabSize_*chunkPos.z +
			rowSize_*chunkPos.y  +
			chunkSize_*chunkPos.x;

		offset += (sz%128)*sliceSize_ + (sy%128)*128 + sx%128;

	}
*/
	void DownsampleChooseOneVoxelOfNine()
	{
		for(uint64_t sz=0; sz < srcDims_.z; sz+=2){
			for(uint64_t sy=0; sy < srcDims_.y; sy+=2){
				for(uint64_t sx=0; sx < srcDims_.x; sx+=2){
					pushVoxelIntoMips(DataCoord(sx, sy, sz));
				}
			}
		}
	}

	void pushVoxelIntoMips(const DataCoord& srcCoord)
	{
		for(int i = 1; i <= maxMipLevel_; ++i){
			if( 0 != srcCoord.z % mipsFactor_[i] ||
				0 != srcCoord.y % mipsFactor_[i] ||
				0 != srcCoord.x % mipsFactor_[i] ){
				return;
			}

			const DataCoord dstCoord = srcCoord / mipsFactor_[i];

		}

	}
};

#endif
