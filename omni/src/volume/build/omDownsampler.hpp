#ifndef OM_DOWNSAMPLER_HPP
#define OM_DOWNSAMPLER_HPP

#include "volume/omMipVolume.h"
#include "utility/omTimer.h"
#include "zi/omThreads.h"
#include "utility/omThreadPool.hpp"

template <typename T>
struct MipLevelInfo {
	T* data;
	uint64_t factor;
	Vector3<uint64_t> volDims;

	uint64_t maxAllowedIndex;
	uint64_t volSlabSize;
	uint64_t volSliceSize;
	uint64_t volRowSize;
	uint64_t totalVoxels;
};

struct MippingInfo {
	int maxMipLevel;
	Vector3<uint64_t> chunkDims;
	uint64_t chunkSize;
	uint64_t tileSize;
};

template <typename T>
class DownsampleVoxelTask : public zi::runnable {
private:
	const std::vector<MipLevelInfo<T> >& mips_;
	const MippingInfo& mippingInfo_;
	const uint64_t sy_;
	const uint64_t sz_;

public:
	DownsampleVoxelTask(const std::vector<MipLevelInfo<T> >& mips,
						const MippingInfo& mippingInfo,
						const uint64_t sy, const uint64_t sz)
		: mips_(mips)
		, mippingInfo_(mippingInfo)
		, sy_(sy), sz_(sz)
	{}

	void run()
	{
		for(uint64_t sx=0; sx < mips_[0].volDims.x; sx+=2){
			pushVoxelIntoMips(DataCoord(sx, sy_, sz_));
		}
	}

	inline uint64_t getOffsetIntoVolume(const DataCoord& coord, const int level)
	{
		const DataCoord chunkPos(coord.x / mippingInfo_.chunkDims.x,
								 coord.y / mippingInfo_.chunkDims.y,
								 coord.z / mippingInfo_.chunkDims.z);

		const uint64_t chunkOffsetFromVolStart =
			mips_[level].volSlabSize * chunkPos.z +
			mips_[level].volRowSize  * chunkPos.y +
			mippingInfo_.chunkSize * chunkPos.x;

		const Vector3i offsetVec = coord - chunkPos*mippingInfo_.chunkDims;

		const uint64_t offsetIntoChunk =
			mippingInfo_.tileSize    * offsetVec.z +
			mippingInfo_.chunkDims.x * offsetVec.y +
			offsetVec.x;

		return chunkOffsetFromVolStart + offsetIntoChunk;
	}

	inline void pushVoxelIntoMips(const DataCoord& srcCoord)
	{
		const uint64_t srcOffset = getOffsetIntoVolume(srcCoord, 0);

		for(int i = 1; i <= mippingInfo_.maxMipLevel; ++i){
			if( 0 != srcCoord.z % mips_[i].factor ||
				0 != srcCoord.y % mips_[i].factor ||
				0 != srcCoord.x % mips_[i].factor ){
				return;
			}

			const DataCoord dstCoord = srcCoord / mips_[i].factor;
			const uint64_t dstOffset = getOffsetIntoVolume(dstCoord, i);
			mips_[i].data[dstOffset] = mips_[0].data[srcOffset];
		}
	}
};

template <typename T>
class OmDownsampler {
private:
	OmMipVolume *const vol_;
	std::vector<MipLevelInfo<T> > mips_;
	MippingInfo mippingInfo_;

public:
	OmDownsampler(OmMipVolume* vol)
		: vol_(vol)
	{
		mippingInfo_.maxMipLevel = vol_->GetRootMipLevel();

		const Vector3<uint64_t> chunkDims = vol_->GetChunkDimensions();

		mippingInfo_.chunkDims = chunkDims;
		mippingInfo_.chunkSize = chunkDims.x * chunkDims.y * chunkDims.z;
		mippingInfo_.tileSize = chunkDims.x * chunkDims.y;

		mips_.resize(mippingInfo_.maxMipLevel + 1);

		for(int i=0; i <= mippingInfo_.maxMipLevel; ++i){
			mips_[i].data = boost::get<T*>(vol_->getVolData()->GetVolPtr(i));
			mips_[i].factor = om::pow2int(i);

			const Vector3i dims = vol_->getDimsRoundedToNearestChunk(i);
			mips_[i].volDims = dims;
			mips_[i].volSlabSize = dims.x * dims.y * chunkDims.z;
			mips_[i].volSliceSize = dims.x * dims.y;
			mips_[i].volRowSize = dims.x * chunkDims.y * chunkDims.z;
			mips_[i].totalVoxels = dims.x * dims.y * dims.z;
		}
	}

	void DownsampleChooseOneVoxelOfNine()
	{
		OmTimer timer;
		printf("downsampling...\n");

		OmThreadPool threadPool;
		threadPool.start();

		for(uint64_t sz=0; sz < mips_[0].volDims.z; sz+=2){
			for(uint64_t sy=0; sy < mips_[0].volDims.y; sy+=2){
					boost::shared_ptr<DownsampleVoxelTask<T> > task =
						boost::make_shared<DownsampleVoxelTask<T> >(mips_,
																	mippingInfo_,
																	sy, sz);
					threadPool.addTaskBack(task);
			}
		}

		threadPool.join();
		printf("\t downsampling done in %f secs\n", timer.s_elapsed());
	}
};

#endif
