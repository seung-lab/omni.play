#pragma once

#include "common/common.h"
#include "datalayer/dataWrapper.h"

namespace om {
namespace chunks {

class dataInterface {
public:
    virtual boost::shared_ptr<uint8_t> ExtractDataSlice8bit(const common::viewType, const int) = 0;

    virtual void CopyInTile(const int sliceOffset, char const*const bits) = 0;

    virtual double GetMinValue() = 0;
    virtual double GetMaxValue() = 0;

    virtual bool Compare(dataInterface const*const other) = 0;
};

} // namespace chunk
} // namespace om
