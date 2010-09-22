#ifndef OM_I_DATA_VOLUME_HPP
#define OM_I_DATA_VOLUME_HPP

#include "common/omCommon.h"

class OmMipChunkCoord;

template <typename T, typename VOL>
class OmIDataVolume {
public:
	virtual ~OmIDataVolume(){}

	virtual void Load() = 0;
	virtual void Create(const std::map<int, Vector3i> &) = 0;
	virtual T* GetPtr(const int level) const = 0;
	virtual T* GetChunkPtr(const OmMipChunkCoord & coord) const = 0;
	virtual int GetBytesPerSample() const = 0;
};

#endif
