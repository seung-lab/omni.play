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

	void loadMemMapFiles();
	void allocMemMapFiles(const std::map<int, Vector3i> & levDims);
	OmVolDataType determineOldVolType(OmMipVolume * vol);

	template <typename VOL> void setDataType(VOL* vol){
		printf("setting up volume data...\n");

		if(OmVolDataType::UNKNOWN == vol->mVolDataType.index()){
			printf("unknown data type--old file? attempting to infer type...\n");
			vol->mVolDataType = determineOldVolType(vol);
		}

		switch(vol->mVolDataType.index()){
		case OmVolDataType::OM_INT8:
			volData_ = OmMemMappedVolume<int8_t, VOL>(vol);
			return;
		case OmVolDataType::OM_UINT8:
			volData_ = OmMemMappedVolume<uint8_t, VOL>(vol);
			return;
		case OmVolDataType::OM_INT32:
			volData_ = OmMemMappedVolume<int32_t, VOL>(vol);
			return;
		case OmVolDataType::OM_UINT32:
			volData_ = OmMemMappedVolume<uint32_t, VOL>(vol);
			return;
		case OmVolDataType::OM_FLOAT:
			volData_ = OmMemMappedVolume<float, VOL>(vol);
			return;
		case OmVolDataType::UNKNOWN:
			assert(0 && "unknown data type--probably old file?");
			return;
		}

		assert(0 && "type not know");
	}
};

#endif
