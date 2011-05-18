#pragma once

#include "volume/omVolumeTypes.hpp"

namespace om {
namespace chunk {

class ptrToChunkDataBase {
public:
    virtual void Release()
    {}

    virtual int8_t* GetRawData(int8_t*){
        return NULL;
    }

    virtual uint8_t* GetRawData(uint8_t*){
        return NULL;
    }

    virtual int32_t* GetRawData(int32_t*){
        return NULL;
    }

    virtual uint32_t* GetRawData(uint32_t*){
        return NULL;
    }

    virtual float* GetRawData(float*){
        return NULL;
    }
};

template <typename DATA>
class dataAccessor {
private:
    ptrToChunkDataBase *const ptr_;
    DATA *const data_;

public:
    dataAccessor(ptrToChunkDataBase* ptr)
        : ptr_(ptr)
        , data_(ptr_->GetRawData(static_cast<DATA*>(NULL)))
    {}

    ~dataAccessor(){
        ptr_->Release();
    }

    DATA* Data(){
        return data_;
    }
};

} // namespace chunk
} // namespace om
