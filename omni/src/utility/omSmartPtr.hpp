#ifndef OM_SMART_PTR_HPP
#define OM_SMART_PTR_HPP

#include "common/omException.h"
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

template <typename T>
class OmSmartPtr {
public:
	static boost::shared_ptr<T> makeMallocPtr(const uint32_t size){
		T* rawPtr = (T*)malloc(size);
		if(NULL == rawPtr){
			throw OmIoException("could not allocate "
					    + boost::lexical_cast<std::string>(size)
					    +" bytes of data");
		}
		return boost::shared_ptr<T>(rawPtr, deallocatorMalloc());
	}
private:
	struct deallocatorMalloc {
		void operator()(T* ptr){
			free(ptr);
		}
	};
};

#endif
