#pragma once
#include "precomp.h"

#include "volume/omVolumeTypes.hpp"

namespace om {
namespace chunk {

// TODO: Get rid of this!
class ptrToChunkDataBase {
 public:
  virtual ~ptrToChunkDataBase() {}

  virtual void Release() {}

  virtual int8_t* GetRawData(int8_t*) { return nullptr; }

  virtual uint8_t* GetRawData(uint8_t*) { return nullptr; }

  virtual int32_t* GetRawData(int32_t*) { return nullptr; }

  virtual uint32_t* GetRawData(uint32_t*) { return nullptr; }

  virtual float* GetRawData(float*) { return nullptr; }
};

template <typename DATA>
class dataAccessor {
 private:
  ptrToChunkDataBase* const ptr_;
  DATA* const data_;

 public:
  dataAccessor(ptrToChunkDataBase* ptr)
      : ptr_(ptr), data_(ptr_->GetRawData(static_cast<DATA*>(nullptr))) {}

  ~dataAccessor() { ptr_->Release(); }

  DATA* Data() { return data_; }
};

}  // namespace chunk
}  // namespace om
