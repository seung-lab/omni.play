#pragma once

#include "common/common.h"
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
    const om::coords::Chunk coord_;
    const int numElementsPerSlice_;
    const int numElementsPerChunk_;
    DATA *const ptrToChunkData_;

public:
    dataImpl(OmMipVolume* vol, const om::coords::Chunk& coord)
        : vol_(vol)
        , coord_(coord)
        , numElementsPerSlice_(128*128)
        , numElementsPerChunk_(numElementsPerSlice_*128)
        , ptrToChunkData_(boost::get<DATA*>(vol->VolData()->getChunkPtrRaw(coord)))
    {}

    void CopyInTile(const int sliceOffset, uchar const*const bits)
    {
        const int advance = (numElementsPerSlice_ * (sliceOffset % 128)) * sizeof(DATA);

        memcpy(ptrToChunkData_ + advance,
               bits,
               numElementsPerSlice_ * sizeof(DATA));
    }

    void CopyInChunkData(OmDataWrapperPtr hdf5)
    {
        DATA const*const dataHDF5 = hdf5->getPtr<DATA>();

        memcpy(ptrToChunkData_,
               dataHDF5,
               numElementsPerChunk_ * sizeof(DATA));
    }

    bool Compare(dataInterface const*const chunkInter)
    {
        dataImpl const*const other = reinterpret_cast<dataImpl const*const>(chunkInter);

        for(int i = 0; i < numElementsPerChunk_; ++i)
        {
            if(other->ptrToChunkData_[i] != ptrToChunkData_[i]){
                return false;
            }
        }

        return true;
    }

    double GetMaxValue() {
        return *std::max_element(ptrToChunkData_, ptrToChunkData_ + numElementsPerChunk_);
    }

    double GetMinValue() {
        return *std::min_element(ptrToChunkData_, ptrToChunkData_ + numElementsPerChunk_);
    }

    OmDataWrapperPtr CopyOutChunkData()
    {
        OmRawChunk<DATA> rawChunk(vol_, coord_);
        return om::ptrs::Wrap(rawChunk.SharedPtr());
    }

    OmPooledTile<uint8_t>* ExtractDataSlice8bit(const om::common::ViewType plane, const int depth)
    {
        extractChanTile extractor(vol_, coord_, plane, depth);
        return extractor.Extract(ptrToChunkData_);
    }
};

} // namespace chunk
} // namespace om
