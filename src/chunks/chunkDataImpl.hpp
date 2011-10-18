#pragma once

#include "common/common.h"
#include "chunks/details/ptrToChunkDataMemMapVol.h"
#include "chunks/chunkDataInterface.hpp"
#include "chunks/extractChanTile.hpp"
#include "chunks/rawChunk.hpp"

namespace om {
namespace chunks {

template <typename DATA>
class dataImpl : public dataInterface {
private:
    volume::volume *const vol_;
    const coords::chunkCoord coord_;
    const int numElementsPerSlice_;
    const int numElementsPerChunk_;
    ptrToChunkDataBase *const ptrToChunkData_;

public:
    dataImpl(volume::volume* vol, const coords::chunkCoord& coord)
        : vol_(vol)
        , coord_(coord)
        , numElementsPerSlice_(128*128)
        , numElementsPerChunk_(numElementsPerSlice_*128)
        , ptrToChunkData_(new ptrToChunkDataMemMapVol<DATA>(vol, coord))
    {}

    ~dataImpl(){
        delete ptrToChunkData_;
    }

    void CopyInTile(const int sliceOffset, char const*const bits)
    {
        const int advance = (numElementsPerSlice_ * (sliceOffset % 128)) * sizeof(DATA);

        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        memcpy(data + advance,
               bits,
               numElementsPerSlice_ * sizeof(DATA));
    }

    bool Compare(dataInterface const*const chunkInter)
    {
        dataImpl const*const other = reinterpret_cast<dataImpl const*const>(chunkInter);

        dataAccessor<DATA> dataWrapperOther(other->ptrToChunkData_);
        DATA const*const otherD = dataWrapperOther.Data();

        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA const*const data = dataWrapper.Data();

        for(int i = 0; i < numElementsPerChunk_; ++i)
        {
            if(otherD[i] != data[i]){
                return false;
            }
        }


        return true;
    }

    double GetMaxValue()
    {
        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA const*const data = dataWrapper.Data();

        return *std::max_element(data, data + numElementsPerChunk_);
    }

    double GetMinValue()
    {
        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA const*const data = dataWrapper.Data();

        return *std::min_element(data, data + numElementsPerChunk_);
    }

    boost::shared_ptr<uint32_t> ExtractDataSlice8bit(const common::viewType plane, const int depth)
    {
        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        extractChanTile extractor(vol_, coord_, plane, depth);
        return extractor.Extract(data);
    }
};

} // namespace chunk
} // namespace om
