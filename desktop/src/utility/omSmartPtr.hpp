#pragma once

#include "common/om.hpp"
#include "common/omString.hpp"
#include "common/omCommon.h"

template <typename T> class OmSmartPtr {
 public:
  static om::shared_ptr<T> inline MallocNumBytes(
      const uint64_t numBytes, const om::ZeroMem shouldZeroMem) {
    T* rawPtr = static_cast<T*>(malloc(numBytes));

    if (!rawPtr) {
      const std::string err =
          "could not allocate " + om::string::num(numBytes) + " bytes of data";
      std::cerr << err << "\n" << std::flush;
      throw std::bad_alloc();
    }

    if (om::ZERO_FILL == shouldZeroMem) {
      memset(rawPtr, 0, numBytes);
    }

    return WrapMalloc(rawPtr);
  }

  static om::shared_ptr<T> inline MallocNumElements(
      const uint64_t numElements, const om::ZeroMem shouldZeroMem) {
    return MallocNumBytes(numElements * sizeof(T), shouldZeroMem);
  }

  static om::shared_ptr<T> inline NewNumElements(const uint64_t numElements) {
    return WrapNewArray(new T[numElements]);
  }

  static om::shared_ptr<T> inline WrapNewArray(T* rawPtr) {
    return om::shared_ptr<T>(rawPtr, deallocatorNewArray());
  }

  static om::shared_ptr<T> inline WrapNoFree(T* rawPtr) {
    return om::shared_ptr<T>(rawPtr, deallocatorDoNothing());
  }

  static om::shared_ptr<T> inline WrapMalloc(T* rawPtr) {
    return om::shared_ptr<T>(rawPtr, deallocatorMalloc());
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
