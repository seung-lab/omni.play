#include "volume/build/omVolumeBuilder.hpp"

OmDataWrapperPtr OmVolumeBuilder::SubsampleData(OmDataWrapperPtr data,
						const DataBbox& src_extents)
{
	switch(data->getVolDataType().index()){
	case OmVolDataType::INT8:
		return doSubsampleData<int8_t>(data, src_extents);
	case OmVolDataType::UINT8:
		return doSubsampleData<uint8_t>(data, src_extents);
	case OmVolDataType::INT32:
		return doSubsampleData<int32_t>(data, src_extents);
	case OmVolDataType::UINT32:
		return doSubsampleData<uint32_t>(data, src_extents);
	case OmVolDataType::FLOAT:
		return doSubsampleData<float>(data, src_extents);
	case OmVolDataType::UNKNOWN:
		throw OmIoException("unknown data type--probably old file?");
	}

	throw OmIoException("unknown vol data type");
}


template <typename T>
OmDataWrapperPtr OmVolumeBuilder::doSubsampleData(OmDataWrapperPtr src_data_ptr,
						  const DataBbox& src_extents)
{
	const Vector3i src_dims = src_extents.getUnitDimensions();
	const Vector3i dest_dims = src_dims / 2;
	std::cout << "Subsample src dims: " << src_dims
		  << "; dest dims: " << dest_dims << "\n";

	const size_t  numElements = dest_dims.x * dest_dims.y * dest_dims.z;
	boost::shared_ptr<T> dest_data_ptr =
		OmSmartPtr<T>::makeMallocPtrNumElements(numElements);

	T* src_data = src_data_ptr->getPtr<T>();
	T* dest_data = dest_data_ptr.get();

	const int sliceSize = src_dims.x * src_dims.y;

	for (int si=0,di=0,dz=0; dz < dest_dims.z; ++dz,si+=sliceSize)
		for (int dy=0; dy < dest_dims.y; ++dy, si+=src_dims.x)
			for (int dx=0; dx < dest_dims.x; ++dx, ++di, si+=2) {
				dest_data[di] = src_data[si];
			}

	return OmDataWrapperFactory::produce(dest_data_ptr);
}
