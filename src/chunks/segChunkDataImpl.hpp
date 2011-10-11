#pragma once

#include "chunks/rawChunk.hpp"
#include "chunks/omProcessChunkVoxelBoundingData.hpp"
#include "chunks/omExtractSegTile.hpp"
#include "chunks/segChunk.h"
#include "segment/segments.h"
#include "utility/chunkVoxelWalker.hpp"
#include "volume/build/omProcessSegmentationChunk.hpp"
#include "volume/segmentation.h"
#include "chunks/details/ptrToChunkDataMemMapVol.h"

namespace om {
namespace segchunk {

using om::chunk::dataAccessor;
using om::chunk::ptrToChunkDataBase;
using om::chunk::ptrToChunkDataMemMapVol;

template <typename DATA>
class dataImpl : public dataInterface {
private:
    segmentation *const vol_;
    segChunk *const chunk_;
    const coords::chunkCoord coord_;

    ptrToChunkDataBase *const ptrToChunkData_;

    const int elementsPerRow_;
    const int elementsPerSlice_;

public:
    dataImpl(segmentation* vol, segChunk* chunk, const coords::chunkCoord& coord)
        : vol_(vol)
        , chunk_(chunk)
        , coord_(coord)
        , ptrToChunkData_(new ptrToChunkDataMemMapVol<DATA>(vol, coord))
        , elementsPerRow_(128)
        , elementsPerSlice_(128*128)
    {}

    ~dataImpl(){
        delete ptrToChunkData_;
    }

    PooledTile32Ptr ExtractDataSlice32bit(const ViewType plane, const int depth)
    {
        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        OmExtractSegTile extractor(vol_, coord_, plane, depth);
        return extractor.Extract(data);
    }

    void ProcessChunk(const bool computeSizes, segments* segments)
    {
        OmProcessSegmentationChunk p(chunk_, computeSizes, segments);

        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        chunkVoxelWalker iter(128);

        for(iter.begin(); iter < iter.end(); ++iter)
        {
            const segId val = static_cast<segId>(*data++);

            if(val){
                p.processVoxel(val, *iter);
            }
        }
    }

    void RefreshBoundingData(segments* segments)
    {
        ProcessChunkVoxelBoundingData p(chunk_, segments);

        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        chunkVoxelWalker iter(128);
        for(iter.begin(); iter < iter.end(); ++iter)
        {
            const segId val = static_cast<segId>(*data++);
            if(val){
                p.processVoxel(val, *iter);
            }
        }
    }

    uint32_t SetVoxelValue(const coords::dataCoord& voxel, const uint32_t val)
    {
        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        const int offset = voxel.toChunkOffset();
        
        const uint32_t oldVal = data[offset];
        data[offset] = val;
        
        return oldVal;
    }

    uint32_t GetVoxelValue(const coords::dataCoord& voxel)
    {
        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        return data[voxel.toChunkOffset()];
    }

    void RewriteChunk(const boost::unordered_map<uint32_t, uint32_t>& vals)
    {
        rawChunk<DATA> rawChunk(vol_, chunk_->GetCoordinate());
        rawChunk.SetDirty();

        DATA* d = rawChunk.Data();

        const int numVoxelsInChunk = chunk_->Mipping().NumVoxels();

        for(int i = 0; i < numVoxelsInChunk; ++i){
            d[i] = vals.at(d[i]);
        }
    }

    om::shared_ptr<uint32_t> GetCopyOfChunkDataAsUint32()
    {
        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        return getChunkAs32bit(data);
    }

private:
    template <typename T>
    om::shared_ptr<uint32_t> getChunkAs32bit(T*) const
    {
        rawChunk<T> rawChunk(vol_, chunk_->GetCoordinate());

        om::shared_ptr<T> data = rawChunk.SharedPtr();
        T* dataRaw = data.get();

        const int numVoxelsInChunk = chunk_->Mipping().NumVoxels();

        om::shared_ptr<uint32_t> ret = OmSmartPtr<uint32_t>::MallocNumElements(numVoxelsInChunk,
                                                                               om::DONT_ZERO_FILL);
        std::copy(dataRaw,
                  dataRaw + numVoxelsInChunk,
                  ret.get());

        return ret;
    }

    om::shared_ptr<uint32_t> getChunkAs32bit(uint32_t*) const
    {
        rawChunk<uint32_t> rawChunk(vol_, chunk_->GetCoordinate());
        return rawChunk.SharedPtr();
    }

    om::shared_ptr<uint32_t> getChunkAs32bit(float*) const {
        throw common::ioException("can't deal with float data!");
    }
};

} // namespace segchunk
} // namespace om
