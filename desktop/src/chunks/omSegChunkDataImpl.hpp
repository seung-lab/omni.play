#pragma once

#include "chunks/omRawChunk.hpp"
#include "chunks/omProcessChunkVoxelBoundingData.hpp"
#include "chunks/omExtractSegTile.hpp"
#include "chunks/omSegChunk.h"
#include "segment/omSegments.h"
#include "utility/omChunkVoxelWalker.hpp"
#include "volume/build/omProcessSegmentationChunk.hpp"
#include "volume/omSegmentation.h"

namespace om {
namespace segchunk {

template <typename DATA>
class dataImpl : public dataInterface {
private:
    OmSegmentation *const vol_;
    OmSegChunk *const chunk_;
    const om::coords::Chunk coord_;

    DATA *const ptrToChunkData_;

    const int elementsPerRow_;
    const int elementsPerSlice_;

public:
    dataImpl(OmSegmentation* vol, OmSegChunk* chunk, const om::coords::Chunk& coord)
        : vol_(vol)
        , chunk_(chunk)
        , coord_(coord)
        , ptrToChunkData_(boost::get<DATA*>(vol->VolData()->getChunkPtrRaw(coord)))
        , elementsPerRow_(128)
        , elementsPerSlice_(128*128)
    {}

    PooledTile32Ptr ExtractDataSlice32bit(const om::common::ViewType plane, const int depth)
    {
        OmExtractSegTile extractor(vol_, coord_, plane, depth);
        return extractor.Extract(ptrToChunkData_);
    }

    void ProcessChunk(const bool computeSizes, OmSegments* segments)
    {
        OmProcessSegmentationChunk p(chunk_, computeSizes, segments);
        OmChunkVoxelWalker iter(128);

        DATA* data = ptrToChunkData_;

        for(iter.begin(); iter < iter.end(); ++iter)
        {
            const om::common::SegID val = static_cast<om::common::SegID>(*data++);

            if(val){
                p.processVoxel(val, *iter);
            }
        }
    }

    void RefreshBoundingData(OmSegments* segments)
    {
        ProcessChunkVoxelBoundingData p(chunk_, segments);
        OmChunkVoxelWalker iter(128);

        DATA* data = ptrToChunkData_;
        for(iter.begin(); iter < iter.end(); ++iter)
        {
            const om::common::SegID val = static_cast<om::common::SegID>(*data++);
            if(val){
                p.processVoxel(val, *iter);
            }
        }
    }

    uint32_t SetVoxelValue(const om::coords::Data& voxel, const uint32_t val)
    {
        const int offset = voxel.ToChunkOffset();

        const uint32_t oldVal = ptrToChunkData_[offset];
        ptrToChunkData_[offset] = val;

        return oldVal;
    }

    uint32_t GetVoxelValue(const om::coords::Data& voxel)
    {
        return ptrToChunkData_[voxel.ToChunkOffset()];
    }

    void RewriteChunk(const boost::unordered_map<uint32_t, uint32_t>& vals)
    {
        OmRawChunk<DATA> rawChunk(vol_, chunk_->GetCoordinate());
        rawChunk.SetDirty();

        DATA* d = rawChunk.Data();

        const int numVoxelsInChunk = chunk_->Mipping().NumVoxels();

        for(int i = 0; i < numVoxelsInChunk; ++i){
            d[i] = vals.at(d[i]);
        }
    }

    boost::shared_ptr<uint32_t> GetCopyOfChunkDataAsUint32()
    {
        return getChunkAs32bit(ptrToChunkData_);
    }

private:
    template <typename T>
    boost::shared_ptr<uint32_t> getChunkAs32bit(T*) const
    {
        OmRawChunk<T> rawChunk(vol_, chunk_->GetCoordinate());

        boost::shared_ptr<T> data = rawChunk.SharedPtr();
        T* dataRaw = data.get();

        const int numVoxelsInChunk = chunk_->Mipping().NumVoxels();

        boost::shared_ptr<uint32_t> ret = OmSmartPtr<uint32_t>::MallocNumElements(numVoxelsInChunk,
                                                                               om::common::DONT_ZERO_FILL);
        std::copy(dataRaw,
                  dataRaw + numVoxelsInChunk,
                  ret.get());

        return ret;
    }

    boost::shared_ptr<uint32_t> getChunkAs32bit(uint32_t*) const
    {
        OmRawChunk<uint32_t> rawChunk(vol_, chunk_->GetCoordinate());
        return rawChunk.SharedPtr();
    }

    boost::shared_ptr<uint32_t> getChunkAs32bit(float*) const {
        throw om::IoException("can't deal with float data!");
    }
};

} // namespace segchunk
} // namespace om
