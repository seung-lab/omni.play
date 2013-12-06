#pragma once

#include "common/common.h"
#include "volume/omVolumeTypes.hpp"
#include "utility/malloc.hpp"

#include <QString>

#define OmDataWrapperRaw(c) (OmDataWrapper<int8_t>::produceNoFree(c))
#define OmDataWrapperInvalid() (OmDataWrapper<int8_t>::produceNull())

template <class T>
struct OmVolDataTypeImpl;
template <>
struct OmVolDataTypeImpl<uint32_t> {
  static OmVolDataType getType() { return OmVolDataType::UINT32; }
};
template <>
struct OmVolDataTypeImpl<int32_t> {
  static OmVolDataType getType() { return OmVolDataType::INT32; }
};
template <>
struct OmVolDataTypeImpl<float> {
  static OmVolDataType getType() { return OmVolDataType::FLOAT; }
};
template <>
struct OmVolDataTypeImpl<int8_t> {
  static OmVolDataType getType() { return OmVolDataType::INT8; }
};
template <>
struct OmVolDataTypeImpl<uint8_t> {
  static OmVolDataType getType() { return OmVolDataType::UINT8; }
};

namespace om {
enum OmDataAllocType {
  MALLOC,
  NEW_ARRAY,
  NONE,
  INVALID
};
}

class OmDataWrapperBase {
 public:
  OmDataWrapperBase() {}
  virtual ~OmDataWrapperBase() {}
  typedef std::shared_ptr<OmDataWrapperBase> ptr_type;

  template <class C>
  C* getPtr() {
    return (C*)getVoidPtr();
  }

  virtual void* getVoidPtr() = 0;

  virtual int getSizeof() = 0;
  virtual ptr_type newWrapper(void*, const om::OmDataAllocType) = 0;

  virtual std::string getTypeAsString() = 0;
  virtual OmVolDataType getVolDataType() = 0;
  virtual int getHdf5FileType() = 0;
  virtual int getHdf5MemoryType() = 0;

  virtual void checkIfValid() = 0;

  template <class T>
  friend class OmDataWrapper;
};

typedef std::shared_ptr<OmDataWrapperBase> OmDataWrapperPtr;

template <class T>
class OmDataWrapper : public OmDataWrapperBase {
 public:
  typedef std::shared_ptr<OmDataWrapper<T> > ptr_type;

  explicit OmDataWrapper(std::shared_ptr<T> sptr) : ptr_(sptr) {}

  static OmDataWrapperPtr produceNull() {
    return ptr_type(new OmDataWrapper());
  };

  static OmDataWrapperPtr produce(T* ptr, const om::OmDataAllocType t) {
    return ptr_type(new OmDataWrapper(ptr, t));
  };
  static OmDataWrapperPtr produce(void* ptr, const om::OmDataAllocType t) {
    return produce(static_cast<T*>(ptr), t);
  };

  template <class Tag, class AnyPtr>
  static OmDataWrapperPtr produce(AnyPtr* ptr) {
    return ptr_type(new OmDataWrapper<T>(reinterpret_cast<T*>(ptr), Tag()));
  }

  static OmDataWrapperPtr produceNoFree(T* ptr) {
    return ptr_type(new OmDataWrapper(ptr, om::NONE));
  };
  static OmDataWrapperPtr produceNoFree(const char* ptr) {
    return ptr_type(new OmDataWrapper((T*)ptr, om::NONE));
  };

  OmDataWrapperPtr newWrapper(T* ptr, const om::OmDataAllocType dt) {
    OmDataWrapperPtr ret = ptr_type(new OmDataWrapper(ptr, dt));
    ret->checkIfValid();
    return ret;
  }
  OmDataWrapperPtr newWrapper(void* ptr, const om::OmDataAllocType dt) {
    return newWrapper(static_cast<T*>(ptr), dt);
  }

  virtual ~OmDataWrapper() {}

  virtual int getSizeof() { return sizeof(T); }

  template <class C>
  C* getPtr() {
    checkIfValid();
    return static_cast<C*>(ptr_.get());
  }

  void* getVoidPtr() {
    checkIfValid();
    return ptr_.get();
  }

  std::shared_ptr<T> Ptr() { return ptr_; }

  std::string getTypeAsString() {
    return OmVolumeTypeHelpers::GetTypeAsString(getVolDataType());
  }

  QString getTypeAsQString() {
    return QString::fromStdString(getTypeAsString());
  }

  OmVolDataType getVolDataType() { return OmVolDataTypeImpl<T>::getType(); }

  int getHdf5FileType() {
    return OmVolumeTypeHelpers::getHDF5FileType(getVolDataType());
  }

  int getHdf5MemoryType() {
    return OmVolumeTypeHelpers::getHDF5MemoryType(getVolDataType());
  }

 private:
  const std::shared_ptr<T> ptr_;

  static std::shared_ptr<T> wrapRawPtr(T* rawPtr, const om::OmDataAllocType d) {
    switch (d) {
      case om::MALLOC:
        return om::mem::Malloc<T>::WrapMalloc(rawPtr);
      case om::NEW_ARRAY:
        return om::mem::Malloc<T>::WrapNewArray(rawPtr);
      case om::NONE:
        return om::mem::Malloc<T>::WrapNoFree(rawPtr);
      case om::INVALID:
      default:
        throw om::ArgException("can't wrap invalid ptr");
    };
  }

  explicit OmDataWrapper() : ptr_() {}

  OmDataWrapper(T* ptr, const om::OmDataAllocType d)
      : ptr_(wrapRawPtr(ptr, d)) {}

  void checkIfValid() {
    if (!ptr_) {
      throw om::IoException("OmDataWrapper: ptr not valid");
    }
  }
};

namespace om {
namespace ptrs {

template <typename T>
static OmDataWrapperPtr Wrap(std::shared_ptr<T> sptr) {
  return std::make_shared<OmDataWrapper<T> >(sptr);
}

template <typename T>
std::shared_ptr<T> UnWrap(const OmDataWrapperPtr wrap) {
  OmDataWrapper<T>* dataPtrReint =
      reinterpret_cast<OmDataWrapper<T>*>(wrap.get());

  return dataPtrReint->Ptr();
}

}  // ptrs
}  // om
