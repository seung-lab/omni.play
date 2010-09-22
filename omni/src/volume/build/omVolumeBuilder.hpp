#ifndef OM_VOLUME_BUILDER_HPP
#define OM_VOLUME_BUILDER_HPP

#include "datalayer/omDataWrapper.h"
#include "common/omCommon.h"

class OmVolumeBuilder {
public:
	static OmDataWrapperPtr SubsampleData(OmDataWrapperPtr data,
					      const DataBbox& src_extents);

private:
	template <typename T>
	static OmDataWrapperPtr doSubsampleData(OmDataWrapperPtr data,
						const DataBbox& src_extents);

};

#endif
