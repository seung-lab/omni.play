#pragma once

#include "common/common.h"
#include "chunks/details/omPtrToChunkDataMemMapVol.h"
#include "chunks/omChunkDataInterface.hpp"
#include "chunks/omExtractChanTile.hpp"
#include "chunks/omRawChunk.hpp"
#include "datalayer/omDataWrapper.h"

namespace om {
namespace chunk {

template <typename DATA>
class dataImpl : public dataInterface {
private:
    OmMipVolume *const vol_;
    const om::chunkCoord coord_;
    const int numElementsPerSlice_;
    const int numElementsPerChunk_;
    ptrToChunkDataBase *const ptrToChunkData_;

public:
    dataImpl(OmMipVolume* vol, const om::chunkCoord& coord)
        : vol_(vol)
        , coord_(coord)
        , numElementsPerSlice_(128*128)
        , numElementsPerChunk_(numElementsPerSlice_*128)
        , ptrToChunkData_(new ptrToChunkDataMemMapVol<DATA>(vol, coord))
    {}

    ~dataImpl(){
        delete ptrToChunkData_;
    }

    void CopyInTile(const int sliceOffset, uchar const*const bits)
    {
        const int advance = (numElementsPerSlice_ * (sliceOffset % 128)) * sizeof(DATA);

        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        memcpy(data + advance,
               bits,
               numElementsPerSlice_ * sizeof(DATA));
    }

    void CopyInChunkData(OmDataWrapperPtr hdf5)
    {
        DATA const*const dataHDF5 = hdf5->getPtr<DATA>();

        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        memcpy(data,
               dataHDF5,
               numElementsPerChunk_ * sizeof(DATA));
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

    OmDataWrapperPtr CopyOutChunkData()
    {
        OmRawChunk<DATA> rawChunk(vol_, coord_);
        return om::ptrs::Wrap(rawChunk.SharedPtr());
    }

    OmPooledTile<uint8_t>* ExtractDataSlice8bit(const ViewType plane, const int depth)
    {
        dataAccessor<DATA> dataWrapper(ptrToChunkData_);
        DATA* data = dataWrapper.Data();

        extractChanTile extractor(vol_, coord_, plane, depth);
        return extractor.Extract(data);
    }
};

} // namespace chunk
} // namespace om
