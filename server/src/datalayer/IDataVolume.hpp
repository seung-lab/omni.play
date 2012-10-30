#pragma once

#include "common/common.h"

namespace om {
namespace datalayer {

template <typename T>
class IDataVolume {
public:
    virtual ~IDataVolume(){}

    virtual void Load() = 0;
    virtual void Create(const std::map<int, Vector3i> &) = 0;
    virtual T* GetPtr(const int level) const = 0;
    //virtual T* GetChunkPtr(const coords::Chunk & coord) const = 0;
    virtual int GetBytesPerVoxel() const = 0;
};

}
}
