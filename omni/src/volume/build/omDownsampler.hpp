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
	const uint64_t tileSize_;
	const uint64_t slabSize_;
	const uint64_t sliceSize_;
	const uint64_t rowSize_;
	const uint64_t chunkSize_;
	const uint64_t totalVoxels_;

public:
	OmDownsampler(OmMipVolume* vol)
		: vol_(vol)
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
		mips_.resize(vol_->GetRootMipLevel() + 1);
		for(int i=0; i <= vol_->GetRootMipLevel(); ++i){
			mips_[i] = boost::get<T*>(vol_->getVolData()->GetVolPtr(i));
			assert(mips_[i]);
		}


//		const Vector3<int64_t> chunkPos = coord.Coordinate; // bottom left corner
//		const int64_t offset =
//			slabSize*chunkPos.z + rowSize*chunkPos.y + chunkSize*chunkPos.x;
	}

/*
	uint64_t getOffsetIntoVolume(uint64_t x, uint64_t y, uint64_t z, int level)
	{
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
		for( int level = 1; level <= vol_->GetRootMipLevel(); ++level){

			const uint64_t numToSkip = OMPOW(2, level);

			// downsample chunk
			for(uint64_t sz=0; sz < srcDims_.z; sz+=numToSkip){
				for(uint64_t sy=0; sy < srcDims_.y; sy+=numToSkip){
					for(uint64_t sx=0; sx < srcDims_.x; sx+=numToSkip){

						const Vector3i chunkPos = (ROUNDDOWN(sx, chunkDims_.x),
												   ROUNDDOWN(sy, chunkDims_.y),
												   ROUNDDOWN(sz, chunkDims_.z));
						uint64_t offset =
							slabSize_*chunkPos.z +
							rowSize_*chunkPos.y  +
							chunkSize_*chunkPos.x;

						offset += (sz%128)*sliceSize_ + (sy%128)*128 + sx%128;

					}
				}
			}
		}
	}
};

#endif
