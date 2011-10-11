#pragma once

#include "common/string.hpp"
#include "common/common.h"
#include "common/enums.hpp"

namespace om {
namespace utility {

template <typename T>
class smartPtr {
public:
    static boost::shared_ptr<T>
    inline MallocNumBytes(const uint64_t numBytes,
                          const common::ZeroMem shouldZeroMem)
    {
        T* rawPtr = static_cast<T*>(malloc(numBytes));

        if(!rawPtr)
        {
            const std::string err = "could not allocate "
                + om::string::num(numBytes)
                +" bytes of data";
            std::cerr << err << "\n" << std::flush;
            throw std::bad_alloc();
        }

        if(common::ZERO_FILL == shouldZeroMem){
            memset(rawPtr, 0, numBytes);
        }

        return WrapMalloc(rawPtr);
    }

    static boost::shared_ptr<T>
    inline MallocNumElements(const uint64_t numElements,
                             const common::ZeroMem shouldZeroMem)
    {
        return MallocNumBytes(numElements * sizeof(T), shouldZeroMem);
    }

    static boost::shared_ptr<T>
    inline NewNumElements(const uint64_t numElements)
    {
        return WrapNewArray(new T[numElements]);
    }

    static boost::shared_ptr<T>
    inline WrapNewArray(T* rawPtr){
        return boost::shared_ptr<T>(rawPtr, deallocatorNewArray());
    }

    static boost::shared_ptr<T>
    inline WrapNoFree(T* rawPtr){
        return boost::shared_ptr<T>(rawPtr, deallocatorDoNothing());
    }

    static boost::shared_ptr<T>
    inline WrapMalloc(T* rawPtr){
        return boost::shared_ptr<T>(rawPtr, deallocatorMalloc());
    }

private:
    struct deallocatorDoNothing {
        void operator()(T*){
        }
    };

    struct deallocatorNewArray {
        void operator()(T* ptr){
            delete [] ptr;
        }
    };

    struct deallocatorMalloc {
        void operator()(T* ptr){
            free(ptr);
        }
    };
};

} // namespace utility
} // namespace om
