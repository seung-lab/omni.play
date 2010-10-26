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
		loadMemMapFiles();
		printf("loaded data\n");
	}

	template <typename VOL>
	void create(VOL* vol, const std::map<int, Vector3i> & levDims ){
		setDataType(vol);
		allocMemMapFiles(levDims);
	}

	int GetBytesPerSample() const;
	OmRawDataPtrs GetVolPtr(const int level);
	OmRawDataPtrs getChunkPtrRaw(const OmMipChunkCoord & coord);

	void downsample(OmMipVolume* vol);

	template <typename VOL>
	void setDataType(VOL* vol)
	{
		printf("setting up volume data...\n");

		if(OmVolDataType::UNKNOWN == vol->mVolDataType.index()){
			printf("unknown data type--old file? attempting to infer type...\n");
			vol->mVolDataType = determineOldVolType(vol);
		}

		volData_ = makeVolData(vol);
	}

private:
	OmVolDataSrcs volData_;

	void loadMemMapFiles();
	void allocMemMapFiles(const std::map<int, Vector3i> & levDims);
	OmVolDataType determineOldVolType(OmMipVolume * vol);
	OmRawDataPtrs GetVolPtrType();

	template <typename VOL>
	OmVolDataSrcs makeVolData(VOL* vol)
	{
		switch(vol->mVolDataType.index()){
		case OmVolDataType::INT8:
			return OmMemMappedVolume<int8_t, VOL>(vol);
		case OmVolDataType::UINT8:
			return OmMemMappedVolume<uint8_t, VOL>(vol);
		case OmVolDataType::INT32:
			return OmMemMappedVolume<int32_t, VOL>(vol);
		case OmVolDataType::UINT32:
			return OmMemMappedVolume<uint32_t, VOL>(vol);
		case OmVolDataType::FLOAT:
			return OmMemMappedVolume<float, VOL>(vol);
		case OmVolDataType::UNKNOWN:
			throw OmIoException("unknown data type--probably old file?");
		}

		throw OmArgException("type not know");
	}
};

#endif
