#pragma once

#include "volume/io/omVolumeData.h"
#include "volume/omMipVolume.h"
#include "chunks/details/omPtrToChunkDataBase.hpp"
#include "zi/omMutex.h"

class OmPtrToChunkDataMemMapVol : public OmPtrToChunkDataBase {
private:
    OmMipVolume *const vol_;
    const OmChunkCoord coord_;

    bool loadedData_;
    OmRawDataPtrs rawData_;

    zi::spinlock lock_;

public:
    OmPtrToChunkDataMemMapVol(OmMipVolume* vol, const OmChunkCoord& coord)
        : vol_(vol)
        , coord_(coord)
        , loadedData_(false)
    {}

    inline int8_t* GetRawData(int8_t*){
        return boost::get<int8_t*>(getRawData());
    }

    inline uint8_t* GetRawData(uint8_t*){
        return boost::get<uint8_t*>(getRawData());
    }

    inline int32_t* GetRawData(int32_t*){
        return boost::get<int32_t*>(getRawData());
    }

    inline uint32_t* GetRawData(uint32_t*){
        return boost::get<uint32_t*>(getRawData());
    }

    inline float* GetRawData(float*){
        return boost::get<float*>(getRawData());
    }

private:
    inline OmRawDataPtrs& getRawData()
    {
        zi::guard g(lock_);

        if(!loadedData_)
        {
            loadedData_ = true;
            return rawData_ = vol_->VolData()->getChunkPtrRaw(coord_);
        }

        return rawData_;
    }
};

