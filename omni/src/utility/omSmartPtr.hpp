#ifndef OM_SMART_PTR_HPP
#define OM_SMART_PTR_HPP

#include "common/omException.h"
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

template <typename T>
class OmSmartPtr {
public:
	static boost::shared_ptr<T> makeMallocPtrNumBytes(const uint32_t numBytes){
		T* rawPtr = (T*)malloc(numBytes);
		if(NULL == rawPtr){
			throw OmIoException("could not allocate "
					    + boost::lexical_cast<std::string>(numBytes)
					    +" bytes of data");
		}
		return boost::shared_ptr<T>(rawPtr, deallocatorMalloc());
	}

	static boost::shared_ptr<T> makeMallocPtrNumElements(const uint32_t numElements){
		return makeMallocPtrNumBytes(numElements * sizeof(T));
	}

private:
	struct deallocatorMalloc {
		void operator()(T* ptr){
			free(ptr);
		}
	};
};

#endif
