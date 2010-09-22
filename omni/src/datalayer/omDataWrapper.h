#ifndef OM_DATA_WRAPPER_H
#define OM_DATA_WRAPPER_H

#include "common/omCommon.h"
#include "volume/omVolumeTypes.hpp"

#include <vtkImageData.h>

#define OmDataWrapperRaw(c) (OmDataWrapper<int8_t>::produceNoFree(c))
#define OmDataWrapperInvalid() (OmDataWrapper<int8_t>::produceNull())

template <class T> struct OmVolDataTypeImpl;
template <> struct OmVolDataTypeImpl<uint32_t>{ static OmVolDataType getType() { return OmVolDataType::UINT32;}};
template <> struct OmVolDataTypeImpl<int32_t> { static OmVolDataType getType() { return OmVolDataType::INT32; }};
template <> struct OmVolDataTypeImpl<float>   { static OmVolDataType getType() { return OmVolDataType::FLOAT; }};
template <> struct OmVolDataTypeImpl<int8_t>  { static OmVolDataType getType() { return OmVolDataType::INT8;  }};
template <> struct OmVolDataTypeImpl<uint8_t> { static OmVolDataType getType() { return OmVolDataType::UINT8; }};

enum OmDataAllocType {
	MALLOC,
	NEW,
	NEW_ARRAY,
	VTK,
	NONE,
	INVALID
};

class OmDataWrapperBase {
public:
	OmDataWrapperBase() {}
	virtual ~OmDataWrapperBase() {}
	typedef boost::shared_ptr<OmDataWrapperBase> ptr_type;

	template <class C> C* getPtr() {
		checkIsNotVTKPtr();
		return (C*) getVoidPtr();
		//{assert(0 && "borked\n"); }
	}

	virtual vtkImageData * getVTKptr() = 0;
	virtual void * getVoidPtr() = 0;
	virtual ptr_type SubsampleData() = 0;

	virtual int getSizeof() = 0;
	virtual ptr_type newWrapper(void *, const OmDataAllocType) = 0;

	virtual std::string getTypeAsString() = 0;
	virtual OmVolDataType getVolDataType() = 0;
	virtual int getHdf5FileType() = 0;
	virtual int getHdf5MemoryType() = 0;

	virtual void checkIfValid() = 0;
	virtual void checkIsNotVTKPtr() = 0;
	virtual void checkIsVTKPtr() = 0;

	template <class T> friend class OmDataWrapper;
};

typedef boost::shared_ptr<OmDataWrapperBase> OmDataWrapperPtr;

template <class T>
class OmDataWrapper : public OmDataWrapperBase {
public:
	typedef boost::shared_ptr<OmDataWrapper> ptr_type;

	static OmDataWrapperPtr produceNull() {
		return ptr_type(new OmDataWrapper());
	};
	static OmDataWrapperPtr produce(void *ptr, const OmDataAllocType t){
		return ptr_type(new OmDataWrapper(ptr, t));
	};
	static OmDataWrapperPtr produceNoFree(void *ptr) {
		return ptr_type(new OmDataWrapper(ptr, NONE));
	};
	static OmDataWrapperPtr produceVTK(void *ptr) {
		return ptr_type(new OmDataWrapper(ptr, VTK));
	};

	OmDataWrapperPtr newWrapper(void *ptr, const OmDataAllocType dt){
		OmDataWrapperPtr ret = ptr_type(new OmDataWrapper(ptr, dt));
		ret->checkIfValid();
		return ret;
	}

	virtual ~OmDataWrapper(){
		switch(mOmDataAllocType){
		case MALLOC:
			free(mData);
			break;
		case NEW:
			delete (T*)mData;
			break;
		case NEW_ARRAY:
			delete [] (T*)mData;
			break;
		case VTK:
			getVTKptr()->Delete();
			break;
		case NONE:
		case INVALID:
			break;
		};
	}

	virtual int getSizeof() {
		return sizeof(T);
	}

	template <class C>
	C* getPtr() {
		checkIfValid();
		checkIsNotVTKPtr();
		return static_cast<C*>(mData);
	}
	vtkImageData * getVTKptr(){
		checkIfValid();
		checkIsVTKPtr();
		return (vtkImageData*)mData;
	}
	void * getVoidPtr(){
		checkIfValid();
		return mData;
	}

	OmDataWrapperPtr SubsampleData()
	{
        	vtkImageData * srcData = getVTKptr();

        	//get image data dimensions
        	Vector3i src_dims;
        	srcData->GetDimensions(src_dims.array);
        	const int scalar_type = srcData->GetScalarType();
        	const int num_scalar_components = srcData->GetNumberOfScalarComponents();

        	//assert proper dims
        	assert((src_dims.x == src_dims.y) && (src_dims.y == src_dims.z));
        	assert(src_dims.x % 2 == 0);

        	//alloc dest image data
        	const Vector3i dest_dims = src_dims / 2;

		std::cout << "Subsample dims: " << dest_dims << "\n";

        	vtkImageData *p_dest_data = vtkImageData::New();
        	p_dest_data->SetDimensions(dest_dims.array);
        	p_dest_data->SetScalarType(scalar_type);
        	p_dest_data->SetNumberOfScalarComponents(num_scalar_components);
        	p_dest_data->AllocateScalars();
        	p_dest_data->Update();

        	//get pointer into subsampled data
        	T *dest_data_ptr = static_cast<T*>(p_dest_data->GetScalarPointer());
       		T *src_data_ptr = static_cast<T*>(srcData->GetScalarPointer());

		const int sliceSize = src_dims.x * src_dims.y;

		for (int si=0,di=0,dz=0; dz < dest_dims.z; ++dz,si+=sliceSize)
         		for (int dy=0; dy < dest_dims.y; ++dy, si+=src_dims.x)
           			for (int dx=0; dx < dest_dims.x; ++dx, ++di, si+=2) {
             				dest_data_ptr[di] = src_data_ptr[si];
				}

        	return OmDataWrapper<T>::produceVTK(p_dest_data);
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
	void * mData;
	const OmDataAllocType mOmDataAllocType;

	explicit OmDataWrapper()
		: mData(NULL)
		, mOmDataAllocType(INVALID) {}

	OmDataWrapper( void * ptr, const OmDataAllocType d)
		: mData(ptr)
		, mOmDataAllocType(d) {}

	void checkIfValid(){
		if(INVALID == mOmDataAllocType){
			throw OmIoException("OmDataWrapper: ptr not valid");
		}
	}
	void checkIsVTKPtr(){
		if(VTK != mOmDataAllocType){
			throw OmIoException("OmDataWrapper: not VTK ptr");
		}
	}
	void checkIsNotVTKPtr(){
		if(VTK == mOmDataAllocType){
			throw OmIoException("OmDataWrapper: were not expecting a VTK ptr");
		}
	}
};

#endif
