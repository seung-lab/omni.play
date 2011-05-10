#pragma once

#include "volume/omVolumeTypes.hpp"

class OmPtrToChunkDataBase {
public:
    OmPtrToChunkDataBase()
    {}

    virtual ~OmPtrToChunkDataBase()
    {}

    virtual int8_t* GetRawData(int8_t*) = 0;
    virtual uint8_t* GetRawData(uint8_t*) = 0;
    virtual int32_t* GetRawData(int32_t*) = 0;
    virtual uint32_t* GetRawData(uint32_t*) = 0;
    virtual float* GetRawData(float*) = 0;
};

