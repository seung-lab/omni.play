#ifndef OM_DOWNSAMPLER_HPP
#define OM_DOWNSAMPLER_HPP

#include "volume/omMipVolume.h"
#include "utility/omTimer.h"
#include "zi/omThreads.h"
#include "utility/omThreadPool.hpp"

#include "volume/build/omDownsamplerTypes.hpp"
#include "volume/build/omDownsamplerVoxelTask.hpp"

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
		threadPool.start(3);

		const Vector3i leaf_mip_dims = vol_->MipLevelDimensionsInMipChunks(0);

		//for all chunks
		for (int z = 0; z < leaf_mip_dims.z; ++z) {
			for (int y = 0; y < leaf_mip_dims.y; ++y) {
				for (int x = 0; x < leaf_mip_dims.x; ++x) {
					const OmMipChunkCoord coord(0, x, y, z);
					boost::shared_ptr<DownsampleVoxelTask<T> > task =
						boost::make_shared<DownsampleVoxelTask<T> >(vol_,
																	mips_,
																	mippingInfo_,
																	coord);
					threadPool.addTaskBack(task);
				}
			}
		}

		threadPool.join();
		printf("\t downsampling done in %f secs\n", timer.s_elapsed());
	}
};

#endif
