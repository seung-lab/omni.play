#pragma once

#include "threads/omTaskManager.hpp"
#include "utility/omTimer.hpp"
#include "volume/build/omDownsamplerTypes.hpp"
#include "volume/build/omDownsamplerVoxelTask.hpp"
#include "volume/omMipVolume.h"
#include "zi/omThreads.h"

template <typename T>
class OmDownsampler {
private:
    OmMipVolume *const vol_;
    OmMemMappedVolumeImpl<T> *const files_;

    std::vector<MipLevelInfo> mips_;
    MippingInfo mippingInfo_;

public:
    OmDownsampler(OmMipVolume* vol, OmMemMappedVolumeImpl<T>* files)
        : vol_(vol)
        , files_(files)
    {
        mippingInfo_.maxMipLevel = vol_->Coords().RootMipLevel();

        const Vector3<uint64_t> chunkDims = vol_->Coords().ChunkDimensions();

        mippingInfo_.chunkDims = chunkDims;
        mippingInfo_.chunkSize = chunkDims.x * chunkDims.y * chunkDims.z;
        mippingInfo_.tileSize = chunkDims.x * chunkDims.y;

        mips_.resize(mippingInfo_.maxMipLevel + 1);

        for(int i=0; i <= mippingInfo_.maxMipLevel; ++i)
        {
            mips_[i].factor = om::math::pow2int(i);

            const Vector3i dims = vol_->Coords().getDimsRoundedToNearestChunk(i);
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

        om::shared_ptr<std::deque<om::coords::Chunk> > coordsPtr =
            vol_->MipChunkCoords(0);

        FOR_EACH(iter, *coordsPtr)
        {
            const om::coords::Chunk& coord = *iter;

            om::shared_ptr<DownsampleVoxelTask<T> > task =
                om::make_shared<DownsampleVoxelTask<T> >(vol_,
                                                            mips_,
                                                            mippingInfo_,
                                                            coord,
                                                            files_);
            threadPool.push_back(task);
        }

        threadPool.join();
        printf("\t downsampling done in %f secs\n", timer.s_elapsed());
    }
};

