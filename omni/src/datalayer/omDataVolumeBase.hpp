#ifndef OM_DATA_VOLUME_BASE_HPP
#define OM_DATA_VOLUME_BASE_HPP

#include "common/omCommon.h"

class OmMipChunkCoord;

template <typename T, typename VOL>
class OmDataVolumeBase {
public:
	OmDataVolumeBase(){}
	virtual ~OmDataVolumeBase(){}

	virtual void load() = 0;
	virtual void create(const std::map<int, Vector3i> &) = 0;
	virtual T* getChunkPtr(const OmMipChunkCoord & coord)  = 0;
	virtual int GetBytesPerSample() = 0;
};

#endif
