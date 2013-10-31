#pragma once

#include "common/string.hpp"
#include "common/common.h"
#include "common/enums.hpp"

namespace om {
namespace mem {

enum class ZeroFill {
  DONT, ZERO
};

template <typename T> class Malloc {
 public:
  static std::shared_ptr<T> inline NumBytesZero(const uint64_t numBytes) {
    return NumBytes(numBytes, ZeroFill::ZERO);
  }

  static std::shared_ptr<T> inline NumBytes(const uint64_t numBytes,
                                            const ZeroFill zero) {
    T* rawPtr = static_cast<T*>(malloc(numBytes));
    if (!rawPtr) {
      log_errors(unknown) << "could not allocate " << numBytes
                          << " bytes of data";
      throw std::bad_alloc();
    }
    if (ZeroFill::ZERO == zero) {
      memset(rawPtr, 0, numBytes);
    }
    return WrapMalloc(rawPtr);
  }

  static std::shared_ptr<T> inline NumElementsZero(const uint64_t numElements) {
    return NumElements(numElements, ZeroFill::ZERO);
  }

  static std::shared_ptr<T> inline NumElementsDontZero(const uint64_t numElements) {
    return NumElements(numElements, ZeroFill::DONT);
  }

  static std::shared_ptr<T> inline NumElements(const uint64_t numElements,
                                               const ZeroFill zero) {
    return NumBytes(numElements * sizeof(T), zero);
  }

  static std::shared_ptr<T> inline NewNumElements(const uint64_t numElements) {
    return WrapNewArray(new T[numElements]);
  }

  static std::shared_ptr<T> inline WrapNewArray(T* rawPtr) {
    return std::shared_ptr<T>(rawPtr, deallocatorNewArray());
  }

  static std::shared_ptr<T> inline WrapNoFree(T* rawPtr) {
    return std::shared_ptr<T>(rawPtr, deallocatorDoNothing());
  }

  static std::shared_ptr<T> inline WrapMalloc(T* rawPtr) {
    return std::shared_ptr<T>(rawPtr, deallocatorMalloc());
  }

 private:
  struct deallocatorDoNothing {
    void operator()(T*) {}
  };

  struct deallocatorNewArray {
    void operator()(T* ptr) { delete[] ptr; }
  };

  struct deallocatorMalloc {
    void operator()(T* ptr) { free(ptr); }
  };
};
}
}  // namespace om::mem::
