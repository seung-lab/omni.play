#pragma once

#include "common/omCommon.h"

template <typename T>
class OmIDataVolume {
public:
	virtual ~OmIDataVolume(){}

	virtual void Load() = 0;
	virtual void Create(const std::map<int, Vector3i> &) = 0;
	virtual T* GetPtr(const int level) const = 0;
	virtual T* GetChunkPtr(const om::chunkCoord & coord) const = 0;
	virtual int GetBytesPerVoxel() const = 0;
};

