#ifndef OM_SMART_PTR_HPP
#define OM_SMART_PTR_HPP

#include "common/om.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string.h>

template <typename T>
class OmSmartPtr {
public:
	static boost::shared_ptr<T>
	makeMallocPtrNumBytes(const uint32_t numBytes,
			      const OM::zeroMem shouldZeroMem
			      = OM::DONT_ZERO_FILL)
	{
		T* rawPtr = static_cast<T*>(malloc(numBytes));
		if(NULL == rawPtr){
			const std::string err = "could not allocate "
				+ boost::lexical_cast<std::string>(numBytes)
				+" bytes of data";
			std::cerr << err << "\n" << std::flush;
			throw std::bad_alloc();
		}
		if(OM::ZERO_FILL == shouldZeroMem){
			memset(rawPtr, 0, numBytes);
		}
		return boost::shared_ptr<T>(rawPtr, deallocatorMalloc());
	}

	static boost::shared_ptr<T>
	makeMallocPtrNumElements(const uint32_t numElements,
				 const OM::zeroMem shouldZeroMem
				 = OM::DONT_ZERO_FILL)
	{
		return makeMallocPtrNumBytes(numElements * sizeof(T),
					     shouldZeroMem);
	}

	static boost::shared_ptr<T> wrapNewArray(T* rawPtr){
		return boost::shared_ptr<T>(rawPtr, deallocatorNewArray());
	}

	static boost::shared_ptr<T> wrapNoFree(T* rawPtr){
		return boost::shared_ptr<T>(rawPtr, deallocatorDoNothing());
	}

	static boost::shared_ptr<T> wrapMalloc(T* rawPtr){
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

#endif
