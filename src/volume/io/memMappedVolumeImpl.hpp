#pragma once

#include "datalayer/IDataVolume.hpp"
#include "datalayer/fs/IOnDiskFile.h"
#include "common/debug.h"
#include "datalayer/fs/fileNames.hpp"
#include "datalayer/fs/memMappedFile.hpp"
#include "project/project.h"
#include "volume/channel.h"
#include "volume/segmentation.h"
#include "volume/volumeTypes.h"
#include "volume/io/chunkOffset.hpp"

#include <zi/mutex.hpp>

namespace om {
namespace datalayer {
template <typename T> class IOnDiskFile;
}
namespace volume {

template <typename T>
class memMappedVolumeImpl : public datalayer::IDataVolume<T> {
private:
    volume* vol_;
    std::vector<boost::shared_ptr<datalayer::memMappedFile<T> > > maps_;

public:

    // for boost::varient
    memMappedVolumeImpl()
    {}

    memMappedVolumeImpl(volume* vol)
        : vol_(vol)
    {}

    virtual ~memMappedVolumeImpl()
    {}

    rawDataPtrs GetType() const {
        return (T*)0;
    }

    void Load()
    {
        std::cout << "loaded mem maps\n";

        resizeMapsVector();

        for(size_t level = 0; level < maps_.size(); ++level) {
            maps_[level] = boost::make_shared<datalayer::memMappedFile<T> >(getFileName(level));
        }
    }

    void Create(const std::map<int, Vector3i>& levelsAndDims)
    {
        resizeMapsVector();

        const int64_t bps = GetBytesPerVoxel();

        FOR_EACH(it, levelsAndDims)
        {
            const int level = it->first;
            const Vector3<int64_t> dims = it->second;
            const int64_t size = dims.x * dims.y * dims.z * bps;

            std::cout << "mip " << level << ": size is: "
                      << om::string::humanizeNum(size)
                      << " (" << dims.x
                      << "," << dims.y
                      << "," << dims.z
                      << ")\n";

            maps_[level] = datalayer::memMappedFile<T>::CreateNumBytes(getFileName(level),
                                                                       size);
        }

        printf("OmMemMappedVolume: done allocating data\n");
    }

    T* GetPtr(const int level) const {
        return maps_[level]->GetPtr();
    }

    T* GetChunkPtr(const coords::chunkCoord& coord) const
    {
        const int level = coord.Level;
        const uint64_t offset = ComputeChunkPtrOffsetBytes(vol_, coord);
        T* ret = maps_[level]->GetPtrWithOffset(offset);
        assert(ret);
        return ret;
    }

    int GetBytesPerVoxel() const {
        return sizeof(T);
    }

private:

    void resizeMapsVector(){
        maps_.resize(vol_->CoordinateSystem().GetRootMipLevel() + 1);
    }

    std::string getFileName(const int level) const {
        return datalayer::fileNames::GetMemMapFileName(vol_, level);
    }
};

} // namespace volume
} // namespace om
