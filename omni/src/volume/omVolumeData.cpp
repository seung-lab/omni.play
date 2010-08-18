#include "volume/omVolumeData.hpp"

void OmVolumeData::determineOldVolType()
{
	const OmMipChunkCoord coord(0,0,0,0);
	OmMipChunkPtr chunk;
	vol_->GetChunk(chunk, coord);
	OmDataWrapperPtr data = chunk->RawReadChunkDataHDF5();

	if(data->getHdf5MemoryType() == H5T_NATIVE_FLOAT){
		vol_->mVolDataType = OM_FLOAT;
	}else if( data->getHdf5MemoryType() == H5T_NATIVE_UINT){
		vol_->mVolDataType = OM_UINT32;
	}else if(data->getHdf5MemoryType() == H5T_NATIVE_INT){
		vol_->mVolDataType = OM_INT32;
	}else if(data->getHdf5MemoryType() == H5T_NATIVE_UCHAR){
		vol_->mVolDataType = OM_UINT8;
	}else if(data->getHdf5MemoryType() == H5T_NATIVE_CHAR){
		vol_->mVolDataType = OM_INT8;
	}else{
		assert(0 && "unknown HDF5 type");
	}
}

void OmVolumeData::loadVolData()
{
	printf("setting up volume data...\n");

	if(UNKNOWN == vol_->mVolDataType){
		printf("unknown data type--probably old file? inferring type...\n");
		determineOldVolType();
	}

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
