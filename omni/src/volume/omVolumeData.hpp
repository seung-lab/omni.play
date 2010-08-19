#ifndef OM_VOLUME_DATA_HPP
#define OM_VOLUME_DATA_HPP

#include "datalayer/fs/omMemMappedVolume.hpp"
#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"

class OmVolumeData {
public:
	OmVolumeData(){}

	template <typename VOL>
	void load(VOL* vol){
		setDataType(vol);
		assert(0);
	}

	template <typename VOL>
	void create(VOL* vol, const std::map<int, Vector3i> & levDims ){
		setDataType(vol);
		allocMemMapFiles(levDims);
	}

	int GetBytesPerSample();
	OmRawDataPtrs getChunkPtrRaw(const OmMipChunkCoord & coord);

private:
	boost::variant<OmMemMappedVolume<int8_t, OmChannel>,
		       OmMemMappedVolume<uint8_t, OmChannel>,
		       OmMemMappedVolume<int32_t, OmChannel>,
		       OmMemMappedVolume<uint32_t, OmChannel>,
		       OmMemMappedVolume<float, OmChannel>,
		       OmMemMappedVolume<int8_t, OmSegmentation>,
		       OmMemMappedVolume<uint8_t, OmSegmentation>,
		       OmMemMappedVolume<int32_t, OmSegmentation>,
		       OmMemMappedVolume<uint32_t, OmSegmentation>,
		       OmMemMappedVolume<float, OmSegmentation> > volData_;

	void allocMemMapFiles(const std::map<int, Vector3i> & levDims);

	template <typename VOL> void determineOldVolType(VOL* vol) {
		const OmMipChunkCoord coord(0,0,0,0);
		OmMipChunkPtr chunk;
		vol->GetChunk(chunk, coord);
		OmDataWrapperPtr data = chunk->RawReadChunkDataHDF5();

		if(data->getHdf5MemoryType() == H5T_NATIVE_FLOAT){
			vol->mVolDataType = OM_FLOAT;
		}else if( data->getHdf5MemoryType() == H5T_NATIVE_UINT){
			vol->mVolDataType = OM_UINT32;
		}else if(data->getHdf5MemoryType() == H5T_NATIVE_INT){
			vol->mVolDataType = OM_INT32;
		}else if(data->getHdf5MemoryType() == H5T_NATIVE_UCHAR){
			vol->mVolDataType = OM_UINT8;
		}else if(data->getHdf5MemoryType() == H5T_NATIVE_CHAR){
			vol->mVolDataType = OM_INT8;
		}else{
			assert(0 && "unknown HDF5 type");
		}
	}

	template <typename VOL> void setDataType(VOL* vol){
		printf("setting up volume data...\n");

		if(UNKNOWN == vol->mVolDataType){
			printf("unknown data type--probably old file? inferring type...\n");
			determineOldVolType(vol);
		}

		switch(vol->mVolDataType){
		case OM_INT8:
			volData_ = OmMemMappedVolume<int8_t, VOL>(vol);
			break;
		case OM_UINT8:
			volData_ = OmMemMappedVolume<uint8_t, VOL>(vol);
			break;
		case OM_INT32:
			volData_ = OmMemMappedVolume<int32_t, VOL>(vol);
			break;
		case OM_UINT32:
			volData_ = OmMemMappedVolume<uint32_t, VOL>(vol);
			break;
		case OM_FLOAT:
			volData_ = OmMemMappedVolume<float, VOL>(vol);
			break;
		case UNKNOWN:
			assert(0 && "unknown data type--probably old file?");
			break;
		default:
			assert(0 && "did not recognize vol data type");
			break;
		}
	}
};

#endif
