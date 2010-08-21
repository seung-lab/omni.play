#ifndef OM_DATA_WRAPPER_H
#define OM_DATA_WRAPPER_H

#include "common/omCommon.h"
#include "common/omGl.h"
#include "utility/image/omImage.hpp"
#include "volume/omVolumeTypes.hpp"

#include "boost/shared_ptr.hpp"
#include "hdf5.h"
#include <vtkImageData.h>

typedef hid_t OmHdf5Type;

#define OmDataWrapperRaw(c) (OmDataWrapper<char>::producenofree(c))
#define OmDataWrapperUint(c) (OmDataWrapper<unsigned int>::produce(c))
#define OmDataWrapperInvalid() (OmDataWrapper<char>::produceinvalid())

class OmMipVolume;
class OmMemoryMappedFile
{
public:
	OmMemoryMappedFile(OmMipVolume *) {}

	static OmMemoryMappedFile * FIXME(OmMipVolume * vol) { return new OmMemoryMappedFile(vol); }

	void ref() {}
	void unref() {}
};

template <class T> struct OmHdf5MemoryTypeImpl;
template <> struct OmHdf5MemoryTypeImpl<unsigned int>   { static hid_t getType() { return H5T_NATIVE_UINT;  }};
template <> struct OmHdf5MemoryTypeImpl<int>            { static hid_t getType() { return H5T_NATIVE_INT;   }};
template <> struct OmHdf5MemoryTypeImpl<float>          { static hid_t getType() { return H5T_NATIVE_FLOAT; }};
template <> struct OmHdf5MemoryTypeImpl<char> 	        { static hid_t getType() { return H5T_NATIVE_CHAR;  }};
template <> struct OmHdf5MemoryTypeImpl<int8_t>	        { static hid_t getType() { return H5T_NATIVE_CHAR;  }};
template <> struct OmHdf5MemoryTypeImpl<unsigned char>  { static hid_t getType() { return H5T_NATIVE_UCHAR; }};

template <class T> struct OmHdf5FileTypeImpl;
template <> struct OmHdf5FileTypeImpl<unsigned int>   { static hid_t getType() { return H5T_STD_U32LE;  }};
template <> struct OmHdf5FileTypeImpl<int>            { static hid_t getType() { return H5T_STD_I32LE;  }};
template <> struct OmHdf5FileTypeImpl<float>          { static hid_t getType() { return H5T_IEEE_F32LE; }};
template <> struct OmHdf5FileTypeImpl<char> 	      { static hid_t getType() { return H5T_STD_I8LE;   }};
template <> struct OmHdf5FileTypeImpl<int8_t> 	      { static hid_t getType() { return H5T_STD_I8LE;   }};
template <> struct OmHdf5FileTypeImpl<unsigned char>  { static hid_t getType() { return H5T_STD_U8LE;   }};

template <class T> struct OmTypeAsString;
template <> struct OmTypeAsString<unsigned int>   { static std::string getTypeAsString() { return "unsigned int"; }};
template <> struct OmTypeAsString<int>            { static std::string getTypeAsString() { return "int"; }};
template <> struct OmTypeAsString<float>          { static std::string getTypeAsString() { return "float"; }};
template <> struct OmTypeAsString<char> 	  { static std::string getTypeAsString() { return "char"; }};
template <> struct OmTypeAsString<int8_t> 	  { static std::string getTypeAsString() { return "char"; }};
template <> struct OmTypeAsString<unsigned char>  { static std::string getTypeAsString() { return "unsigned char"; }};

class OmDataWrapperBase {
public:
	OmDataWrapperBase() {}
	virtual ~OmDataWrapperBase() {}
	typedef boost::shared_ptr<OmDataWrapperBase> ptr_type;

	template <class C> C* getPtr() { return (C*) getVoidPtr(); }
	//template <class C> C* getPtr() { return reinterpret_cast<OmDataWrapper<C> >(*this).getPtr(); }
	//template <class C> C* getPtr() {assert(0 && "borked\n"); }

	virtual vtkImageData * getVTKPtr() = 0;
	virtual void * getVoidPtr() = 0;
	virtual ptr_type SubsampleData() = 0;

	virtual int getSizeof() = 0;
	virtual ptr_type newWrapper(void *) = 0;
	virtual bool IsValid() { return false; }

	virtual std::string getTypeAsString() = 0;
	virtual hid_t getHdf5MemoryType() = 0;
	virtual hid_t getHdf5FileType() = 0;
	virtual OmVolDataType getVolDataType() = 0;

	template <class T> friend class OmDataWrapper;
};

typedef boost::shared_ptr<OmDataWrapperBase> OmDataWrapperPtr;

template <class T>
class OmDataWrapper : public OmDataWrapperBase {
private:
	explicit OmDataWrapper(const int m)
		: mData(NULL)
		, isValid(false)
		, mMMFile(NULL)
		, mShouldFree(m) {}

	OmDataWrapper( void * ptr, const int m)
		: mData(ptr)
		, isValid(true)
		, mMMFile(NULL)
		, mShouldFree(m) {}
	OmDataWrapper( void * ptr, const int m, OmMemoryMappedFile* mm)
		: mData(ptr)
		, isValid(true)
		, mMMFile(mm)
		, mShouldFree(m) {}
 public:
	typedef boost::shared_ptr<OmDataWrapper> ptr_type;
	virtual ~OmDataWrapper(){
		//printf("dsy=%p\n", mData);
		if(!isValid) {
			return;
		}
		if(1 == mShouldFree) {
			free(mData); // from malloc in hdf5...
		} else if(2 == mShouldFree){
			getVTKPtr()->Delete();
		} else  if(3 == mShouldFree) {

		} else if (4 == mShouldFree) {
		} else if( -2 == mShouldFree){
			// NULL
		} else {
			assert(0);
			printf("Can not free pointer don't know it's type\n");
		}
	}
	virtual int getSizeof() { return sizeof(T); }
	template <class C>
	C* getPtr()
	{
		assert(isValid);
		return static_cast<C*>(mData);
	}
	vtkImageData * getVTKPtr() { assert(isValid); return (vtkImageData*) mData; }
	virtual void * getVoidPtr(){ assert(isValid); return mData; }
	virtual bool IsValid() { return isValid; }

	static OmDataWrapperPtr produce(void *ptr) {
		return ptr_type(new OmDataWrapper(ptr, 1));
	};
	static OmDataWrapperPtr producenofree(void *ptr) {
		return ptr_type(new OmDataWrapper(ptr, 4));
	};
	static OmDataWrapperPtr produceNull() {
		return ptr_type(new OmDataWrapper(-2));
	};
	static OmDataWrapperPtr producevtk(void *ptr) {
		return ptr_type(new OmDataWrapper(ptr, 2));
	};
	static OmDataWrapperPtr producemmap(void *ptr){
		OmDataWrapper * dw = new OmDataWrapper(ptr, 3);
		return ptr_type(dw);
	}

	static OmDataWrapperPtr produceinvalid() {
		return produceNull();
	}

	OmDataWrapperPtr newWrapper(void *ptr) {
		return ptr_type(new OmDataWrapper(ptr, mShouldFree));
	}

	OmDataWrapperPtr SubsampleData()
	{
        	vtkImageData * srcData = getVTKPtr();

        	//get image data dimensions
        	Vector3 < int >src_dims;
        	srcData->GetDimensions(src_dims.array);
        	int scalar_type = srcData->GetScalarType();
        	int num_scalar_components = srcData->GetNumberOfScalarComponents();

        	//assert proper dims
        	//debug("FIXME", << "OmMipVolume::SubsampleImageData: " << src_dims << endl;
        	assert((src_dims.x == src_dims.y) && (src_dims.y == src_dims.z));
        	assert(src_dims.x % 2 == 0);

        	//alloc dest image data
        	Vector3 < int >dest_dims = src_dims / 2;

		std::cout << "Dims: " << dest_dims << "\n";

        	vtkImageData *p_dest_data = vtkImageData::New();
        	p_dest_data->SetDimensions(dest_dims.array);
        	p_dest_data->SetScalarType(scalar_type);
        	p_dest_data->SetNumberOfScalarComponents(num_scalar_components);
        	p_dest_data->AllocateScalars();
        	p_dest_data->Update();

        	//get pointer into subsampled data
        	T *dest_data_ptr = static_cast < T * >(p_dest_data->GetScalarPointer());
       		T *src_data_ptr = static_cast < T * >(srcData->GetScalarPointer());

		int sliceSize = src_dims.x * src_dims.y;

		for (int si=0,di=0,dz=0; dz < dest_dims.z; ++dz,si+=sliceSize)
         		for (int dy=0; dy < dest_dims.y; ++dy, si+=src_dims.x)
           			for (int dx=0; dx < dest_dims.x; ++dx, ++di, si+=2) {
             				dest_data_ptr[di] = src_data_ptr[si];
           		}

	        //return subsampled image data
        	return OmDataWrapper<T>::producevtk(p_dest_data);
	}

        hid_t getHdf5MemoryType() {return OmHdf5MemoryTypeImpl<T>::getType(); }
	hid_t getHdf5FileType(){ return OmHdf5FileTypeImpl<T>::getType(); }
	std::string getTypeAsString(){ return OmTypeAsString<T>::getTypeAsString(); }

	OmVolDataType getVolDataType(){
		if(getHdf5MemoryType() == H5T_NATIVE_FLOAT){
			return OmVolDataType::OM_FLOAT;
		}else if( getHdf5MemoryType() == H5T_NATIVE_UINT){
			return OmVolDataType::OM_UINT32;
		}else if(getHdf5MemoryType() == H5T_NATIVE_INT){
			return OmVolDataType::OM_INT32;
		}else if(getHdf5MemoryType() == H5T_NATIVE_UCHAR){
			return OmVolDataType::OM_UINT8;
		}else if(getHdf5MemoryType() == H5T_NATIVE_CHAR){
			return OmVolDataType::OM_INT8;
		}else{
			throw OmIoException("unknown HDF5 type");
		}
	}

private:
	void *const mData;
	const bool isValid;
	OmMemoryMappedFile *const mMMFile;
	const int mShouldFree;
};

#endif
