#ifndef OM_VOLUME_BUILDER_HPP
#define OM_VOLUME_BUILDER_HPP

#include "datalayer/omDataWrapper.h"
#include "common/omCommon.h"

class OmMipVolume;

class OmVolumeBuilder {
public:
	template <typename T>
	static void downsample(T* src, T* dst, OmMipVolume* vol,
						   const int preLevel, const int level)
	{
		assert(src && dst && vol);
		printf("downsampling %d to %d\n", preLevel, level);
/*
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
*/
	}
};

#endif
