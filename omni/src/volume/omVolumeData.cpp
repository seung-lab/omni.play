#include "volume/omVolumeData.hpp"

void OmVolumeData::loadVolData()
{
	printf("setting up volume data...\n");

	switch(vol_->mVolDataType){
	case OM_INT8:
		volData = OmMemMappedVolume<int8_t, OmMipVolume>(vol_);
		break;
	case OM_UINT8:
		volData = OmMemMappedVolume<uint8_t, OmMipVolume>(vol_);
		break;
	case OM_INT32:
		volData = OmMemMappedVolume<int32_t, OmMipVolume>(vol_);
		break;
	case OM_UINT32:
		volData = OmMemMappedVolume<uint32_t, OmMipVolume>(vol_);
		break;
	case OM_FLOAT:
		volData = OmMemMappedVolume<float, OmMipVolume>(vol_);
		break;
	case UNKNOWN:
		assert(0 && "unknown data type--probably old file?");
		break;
	default:
		assert(0 && "did not recognize vol data type");
		break;
	}
}
