#pragma once

#include "chunks/rawChunk.hpp"
#include "chunks/extractSegTile.hpp"
#include "chunks/segChunk.h"
//#include "segment/segments.h"
#include "utility/chunkVoxelWalker.hpp"
#include "volume/segmentation.h"
#include "chunks/details/ptrToChunkDataMemMapVol.h"

namespace om {
namespace segchunk {

template <typename DATA>
class dataImpl : public dataInterface {
private:
    volume::segmentation *const vol_;
    chunks::segChunk *const chunk_;
    const coords::chunkCoord coord_;

    chunks::ptrToChunkDataBase *const ptrToChunkData_;

    const int elementsPerRow_;
    const int elementsPerSlice_;

public:
    dataImpl(volume::segmentation* vol,
             chunks::segChunk* chunk,
             const coords::chunkCoord& coord)
        : vol_(vol)
        , chunk_(chunk)
        , coord_(coord)
        , ptrToChunkData_(new chunks::ptrToChunkDataMemMapVol<DATA>(vol, coord))
        , elementsPerRow_(128)
        , elementsPerSlice_(128*128)
    {}

    ~dataImpl(){
        delete ptrToChunkData_;
    }

    boost::shared_ptr<char> ExtractDataSlice32bit(const common::viewType plane, const int depth)
    {
        chunks::dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        chunks::extractSegTile extractor(vol_, coord_, plane, depth);
        return extractor.Extract(data);
    }

    uint32_t SetVoxelValue(const coords::dataCoord& voxel, const uint32_t val)
    {
        chunks::dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        const int offset = voxel.toChunkOffset();

        const uint32_t oldVal = data[offset];
        data[offset] = val;

        return oldVal;
    }

    uint32_t GetVoxelValue(const coords::dataCoord& voxel)
    {
        chunks::dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        return data[voxel.toChunkOffset()];
    }


private:
    template <typename T>
    boost::shared_ptr<uint32_t> getChunkAs32bit(T*) const
    {
        chunks::rawChunk<T> rawChunk(vol_, chunk_->GetCoordinate());

        boost::shared_ptr<T> data = rawChunk.SharedPtr();
        T* dataRaw = data.get();

        const int numVoxelsInChunk = chunk_->Mipping().NumVoxels();

        boost::shared_ptr<uint32_t> ret =
            utility::smartPtr<uint32_t>::MallocNumElements(numVoxelsInChunk,
                                                          common::DONT_ZERO_FILL);

        std::copy(dataRaw,
                  dataRaw + numVoxelsInChunk,
                  ret.get());

        return ret;
    }

    boost::shared_ptr<uint32_t> getChunkAs32bit(uint32_t*) const
    {
        chunks::rawChunk<uint32_t> rawChunk(vol_, chunk_->GetCoordinate());
        return rawChunk.SharedPtr();
    }

    boost::shared_ptr<uint32_t> getChunkAs32bit(float*) const {
        throw common::ioException("can't deal with float data!");
    }
};

} // namespace segchunk
} // namespace om
