#ifndef OM_DATA_WRAPPER_H
#define OM_DATA_WRAPPER_H

#include "common/omCommon.h"
#include "volume/omVolumeTypes.hpp"
#include "utility/omSmartPtr.hpp"

#define OmDataWrapperRaw(c) (OmDataWrapper<int8_t>::produceNoFree(c))
#define OmDataWrapperInvalid() (OmDataWrapper<int8_t>::produceNull())

template <class T> struct OmVolDataTypeImpl;
template <> struct OmVolDataTypeImpl<uint32_t>{ static OmVolDataType getType() { return OmVolDataType::UINT32;}};
template <> struct OmVolDataTypeImpl<int32_t> { static OmVolDataType getType() { return OmVolDataType::INT32; }};
template <> struct OmVolDataTypeImpl<float>   { static OmVolDataType getType() { return OmVolDataType::FLOAT; }};
template <> struct OmVolDataTypeImpl<int8_t>  { static OmVolDataType getType() { return OmVolDataType::INT8;  }};
template <> struct OmVolDataTypeImpl<uint8_t> { static OmVolDataType getType() { return OmVolDataType::UINT8; }};

namespace om {
enum OmDataAllocType {
    MALLOC,
    NEW_ARRAY,
    NONE,
    INVALID
};
}

/* TODO: fixme
   struct malloc_tag    { operator OmDataAllocType() const { return MALLOC; } };
   struct new_array_tag { operator OmDataAllocType() const { return NEW_ARRAY; } };
   struct none_tag      { operator OmDataAllocType() const { return NONE; } };
   struct invalid_tag   { operator OmDataAllocType() const { return INVALID; } };

   namespace om {

   typedef malloc_tag    MALLOC;
   typedef new_array_tag NEW_ARRAY;
   typedef none_tag      NONE;
   typedef invalid_tag   INVALID;

   } // namespace om
*/

class OmDataWrapperBase {
public:
    OmDataWrapperBase() {}
    virtual ~OmDataWrapperBase() {}
    typedef boost::shared_ptr<OmDataWrapperBase> ptr_type;

    template <class C> C* getPtr() {
        return (C*) getVoidPtr();
        //{assert(0 && "borked\n"); }
    }

    virtual void * getVoidPtr() = 0;

    virtual int getSizeof() = 0;
    virtual ptr_type newWrapper(void *, const om::OmDataAllocType) = 0;

    virtual std::string getTypeAsString() = 0;
    virtual OmVolDataType getVolDataType() = 0;
    virtual int getHdf5FileType() = 0;
    virtual int getHdf5MemoryType() = 0;

    virtual void checkIfValid() = 0;

    template <class T> friend class OmDataWrapper;
};

typedef boost::shared_ptr<OmDataWrapperBase> OmDataWrapperPtr;

template <class T>
class OmDataWrapper : public OmDataWrapperBase {
public:
    typedef boost::shared_ptr<OmDataWrapper< T > > ptr_type;

    explicit OmDataWrapper(boost::shared_ptr<T> sptr)
        : ptr_(sptr) {}

    static OmDataWrapperPtr produceNull() {
        return ptr_type(new OmDataWrapper());
    };

    static OmDataWrapperPtr produce(T* ptr, const om::OmDataAllocType t){
        return ptr_type(new OmDataWrapper(ptr, t));
    };
    static OmDataWrapperPtr produce(void* ptr, const om::OmDataAllocType t){
        return produce(static_cast<T*>(ptr), t);
    };

    template< class Tag, class AnyPtr >
    static OmDataWrapperPtr produce( AnyPtr* ptr )
    {
        return ptr_type( new OmDataWrapper< T >( reinterpret_cast< T* >( ptr ), Tag() ) );
    }

    static OmDataWrapperPtr produceNoFree(T* ptr) {
        return ptr_type(new OmDataWrapper(ptr, om::NONE));
    };
    static OmDataWrapperPtr produceNoFree(const char* ptr) {
        return ptr_type(new OmDataWrapper((T*)ptr, om::NONE));
    };

    OmDataWrapperPtr newWrapper(T* ptr, const om::OmDataAllocType dt){
        OmDataWrapperPtr ret = ptr_type(new OmDataWrapper(ptr, dt));
        ret->checkIfValid();
        return ret;
    }
    OmDataWrapperPtr newWrapper(void* ptr, const om::OmDataAllocType dt){
        return newWrapper(static_cast<T*>(ptr), dt);
    }

    virtual ~OmDataWrapper(){}

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

    QString getTypeAsQString(){
        return QString::fromStdString(getTypeAsString());
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

    static boost::shared_ptr<T> wrapRawPtr(T* rawPtr, const om::OmDataAllocType d){
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

    explicit OmDataWrapper()
        : ptr_() {}

    OmDataWrapper(T* ptr, const om::OmDataAllocType d)
        : ptr_(wrapRawPtr(ptr, d)) {}

    void checkIfValid(){
        if(!ptr_){
            throw OmIoException("OmDataWrapper: ptr not valid");
        }
    }
};

namespace om {
namespace ptrs {

template <typename T>
static OmDataWrapperPtr Wrap(boost::shared_ptr<T> sptr){
    return boost::make_shared<OmDataWrapper<T> >(sptr);
}

template <typename T>
boost::shared_ptr<T> UnWrap(const OmDataWrapperPtr wrap)
{
    OmDataWrapper<T>* dataPtrReint =
        reinterpret_cast<OmDataWrapper<T>*>(wrap.get());

    return dataPtrReint->Ptr();
}

} // ptrs
} // om

#endif
