#ifndef OM_DATA_WRAPPER_H
#define OM_DATA_WRAPPER_H

#include "common/omCommon.h"
#include "volume/omVolumeTypes.hpp"

#include <vtkImageData.h>

#define OmDataWrapperRaw(c) (OmDataWrapper<int8_t>::producenofree(c))
#define OmDataWrapperUint(c) (OmDataWrapper<uint32_t>::produce(c))
#define OmDataWrapperInvalid() (OmDataWrapper<int8_t>::produceinvalid())

class OmMipVolume;

template <class T> struct OmVolDataTypeImpl;
template <> struct OmVolDataTypeImpl<uint32_t>{ static OmVolDataType getType() { return OmVolDataType::UINT32;  }};
template <> struct OmVolDataTypeImpl<int32_t> { static OmVolDataType getType() { return OmVolDataType::INT32;   }};
template <> struct OmVolDataTypeImpl<float>   { static OmVolDataType getType() { return OmVolDataType::FLOAT; }};
template <> struct OmVolDataTypeImpl<int8_t>	 { static OmVolDataType getType() { return OmVolDataType::INT8;  }};
template <> struct OmVolDataTypeImpl<uint8_t> { static OmVolDataType getType() { return OmVolDataType::UINT8; }};

class OmDataWrapperBase {
public:
	OmDataWrapperBase() {}
	virtual ~OmDataWrapperBase() {}
	typedef boost::shared_ptr<OmDataWrapperBase> ptr_type;

	template <class C> C* getPtr() { return (C*) getVoidPtr(); }
	//template <class C> C* getPtr() {assert(0 && "borked\n"); }

	virtual vtkImageData * getVTKPtr() = 0;
	virtual void * getVoidPtr() = 0;
	virtual ptr_type SubsampleData() = 0;

	virtual int getSizeof() = 0;
	virtual ptr_type newWrapper(void *) = 0;
	virtual bool IsValid() { return false; }

	virtual std::string getTypeAsString() = 0;
	virtual OmVolDataType getVolDataType() = 0;
	virtual int getHdf5FileType() = 0;
	virtual int getHdf5MemoryType() = 0;

	template <class T> friend class OmDataWrapper;
};

typedef boost::shared_ptr<OmDataWrapperBase> OmDataWrapperPtr;

template <class T>
class OmDataWrapper : public OmDataWrapperBase {
private:
	explicit OmDataWrapper(const int m)
		: mData(NULL)
		, isValid(false)
		, mShouldFree(m) {}

	OmDataWrapper( void * ptr, const int m)
		: mData(ptr)
		, isValid(true)
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

	std::string getTypeAsString(){
		return OmVolumeTypeHelpers::GetTypeAsString(getVolDataType());
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
	void *const mData;
	const bool isValid;
	const int mShouldFree;
};

#endif
