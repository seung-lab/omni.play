#pragma once

#include "datalayer/dataWrapper.h"

template <class> class OmPooledTile;

namespace om {
namespace chunk {

class dataInterface {
public:
    virtual OmPooledTile<uint8_t>* ExtractDataSlice8bit(const ViewType, const int) = 0;

    virtual void CopyInTile(const int sliceOffset, uchar const*const bits) = 0;
    virtual void CopyInChunkData(dataWrapperPtr hdf5) = 0;
    virtual dataWrapperPtr CopyOutChunkData() = 0;

    virtual double GetMinValue() = 0;
    virtual double GetMaxValue() = 0;

    virtual bool Compare(dataInterface const*const other) = 0;
};

} // namespace chunk
} // namespace om
