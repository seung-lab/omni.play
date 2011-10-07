#pragma once

#include "common/common.h"
#include "volume/volumeTypes.h"

#define dataWrapperRaw(c) (dataWrapper<int8_t>::produceNoFree(c))
#define dataWrapperInvalid() (dataWrapper<int8_t>::produceNull())

template <class T> struct OmVolDataTypeImpl;
template <> struct OmVolDataTypeImpl<uint32_t>{ static volDataType getType() { return OmVolDataType::UINT32;}};
template <> struct OmVolDataTypeImpl<int32_t> { static OmVolDataType getType() { return OmVolDataType::INT32; }};
template <> struct OmVolDataTypeImpl<float>   { static OmVolDataType getType() { return OmVolDataType::FLOAT; }};
template <> struct OmVolDataTypeImpl<int8_t>  { static OmVolDataType getType() { return OmVolDataType::INT8;  }};
template <> struct OmVolDataTypeImpl<uint8_t> { static OmVolDataType getType() { return OmVolDataType::UINT8; }};

namespace om {
enum dataAllocType {
    MALLOC,
    NEW_ARRAY,
    NONE,
    INVALID
};
}

/* TODO: fixme
   struct malloc_tag    { operator dataAllocType() const { return MALLOC; } };
   struct new_array_tag { operator dataAllocType() const { return NEW_ARRAY; } };
   struct none_tag      { operator dataAllocType() const { return NONE; } };
   struct invalid_tag   { operator dataAllocType() const { return INVALID; } };

   namespace om {

   typedef malloc_tag    MALLOC;
   typedef new_array_tag NEW_ARRAY;
   typedef none_tag      NONE;
   typedef invalid_tag   INVALID;

   } // namespace om
*/

class dataWrapperBase {
public:
    dataWrapperBase() {}
    virtual ~dataWrapperBase() {}
    typedef boost::shared_ptr<dataWrapperBase> ptr_type;

    template <class C> C* getPtr() {
        return (C*) getVoidPtr();
        //{assert(0 && "borked\n"); }
    }

    virtual void * getVoidPtr() = 0;

    virtual int getSizeof() = 0;
    virtual ptr_type newWrapper(void *, const om::dataAllocType) = 0;

    virtual std::string getTypeAsString() = 0;
    virtual OmVolDataType getVolDataType() = 0;
    virtual int getHdf5FileType() = 0;
    virtual int getHdf5MemoryType() = 0;

    virtual void checkIfValid() = 0;

    template <class T> friend class dataWrapper;
};

typedef boost::shared_ptr<dataWrapperBase> dataWrapperPtr;

template <class T>
class dataWrapper : public dataWrapperBase {
public:
    typedef boost::shared_ptr<dataWrapper< T > > ptr_type;

    explicit dataWrapper(boost::shared_ptr<T> sptr)
        : ptr_(sptr) {}

    static dataWrapperPtr produceNull() {
        return ptr_type(new dataWrapper());
    };

    static dataWrapperPtr produce(T* ptr, const om::dataAllocType t){
        return ptr_type(new dataWrapper(ptr, t));
    };
    static dataWrapperPtr produce(void* ptr, const om::dataAllocType t){
        return produce(static_cast<T*>(ptr), t);
    };

    template< class Tag, class AnyPtr >
    static dataWrapperPtr produce( AnyPtr* ptr )
    {
        return ptr_type( new dataWrapper< T >( reinterpret_cast< T* >( ptr ), Tag() ) );
    }

    static dataWrapperPtr produceNoFree(T* ptr) {
        return ptr_type(new dataWrapper(ptr, om::NONE));
    };
    static dataWrapperPtr produceNoFree(const char* ptr) {
        return ptr_type(new dataWrapper((T*)ptr, om::NONE));
    };

    dataWrapperPtr newWrapper(T* ptr, const om::dataAllocType dt){
        dataWrapperPtr ret = ptr_type(new dataWrapper(ptr, dt));
        ret->checkIfValid();
        return ret;
    }
    dataWrapperPtr newWrapper(void* ptr, const om::dataAllocType dt){
        return newWrapper(static_cast<T*>(ptr), dt);
    }

    virtual ~dataWrapper(){}

    virtual int getSizeof() {
        return sizeof(T);
    }

    template <class C>
    C* getPtr() {
        checkIfValid();
        return static_cast<C*>(ptr_.get());
    }

    void* getVoidPtr(){
        checkIfValid();
        return ptr_.get();
    }

    boost::shared_ptr<T> Ptr(){
        return ptr_;
    }

    std::string getTypeAsString(){
        return OmVolumeTypeHelpers::GetTypeAsString(getVolDataType());
    }

    std::string getTypeAsstd::string(){
        return std::string::fromStdString(getTypeAsString());
    }

    OmVolDataType getVolDataType(){
        return OmVolDataTypeImpl<T>::getType();
    }

    int getHdf5FileType(){
        return OmVolumeTypeHelpers::getHDF5FileType(getVolDataType());
    }

    int getHdf5MemoryType(){
        return OmVolumeTypeHelpers::getHDF5MemoryType(getVolDataType());
    }

private:
    const boost::shared_ptr<T> ptr_;

    static boost::shared_ptr<T> wrapRawPtr(T* rawPtr, const om::dataAllocType d){
        switch(d){
        case om::MALLOC:
            return OmSmartPtr<T>::WrapMalloc(rawPtr);
        case om::NEW_ARRAY:
            return OmSmartPtr<T>::WrapNewArray(rawPtr);
        case om::NONE:
            return OmSmartPtr<T>::WrapNoFree(rawPtr);
        case om::INVALID:
        default:
            throw OmArgException("can't wrap invalid ptr");
        };
    }

    explicit dataWrapper()
        : ptr_() {}

    dataWrapper(T* ptr, const om::dataAllocType d)
        : ptr_(wrapRawPtr(ptr, d)) {}

    void checkIfValid(){
        if(!ptr_){
            throw OmIoException("dataWrapper: ptr not valid");
        }
    }
};

namespace om {
namespace ptrs {

template <typename T>
static dataWrapperPtr Wrap(boost::shared_ptr<T> sptr){
    return om::make_shared<dataWrapper<T> >(sptr);
}

template <typename T>
boost::shared_ptr<T> UnWrap(const dataWrapperPtr wrap)
{
    dataWrapper<T>* dataPtrReint =
        reinterpret_cast<dataWrapper<T>*>(wrap.get());

    return dataPtrReint->Ptr();
}

} // ptrs
} // om

