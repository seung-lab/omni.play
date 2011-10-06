#pragma once

#include "common/debug.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/memMappedFileQT.hpp"
#include "datalayer/IDataVolume.hpp"
#include "datalayer/IDataVolume.hpp"
#include "project/project.h"
#include "utility/omStringHelpers.h"
#include "volume/io/chunkOffset.hpp"
#include "volume/channel.h"
#include "volume/segmentation.h"
#include "volume/volumeTypes.h"

#include <zi/mutex.hpp>
#include <QFile>

template <typename T> class IOnDiskFile;

template <typename T>
class memMappedVolumeImpl : public IDataVolume<T> {
private:
    mipVolume* vol_;
    std::vector<boost::shared_ptr<IOnDiskFile<T> > > maps_;

    typedef OmMemMappedFileReadQT<T> reader_t;
    typedef OmMemMappedFileWriteQT<T> writer_t;

public:

    // for boost::varient
    memMappedVolumeImpl()
    {}

   memMappedVolumeImpl(mipVolume* vol)
        : vol_(vol)
    {}

    virtual ~memMappedVolumeImpl()
    {}

    OmRawDataPtrs GetType() const {
        return (T*)0;
    }

    void Load()
    {
        std::cout << "loaded mem maps\n";

        resizeMapsVector();

        for(size_t level = 0; level < maps_.size(); ++level) {
            maps_[level] = reader_t::Reader(getFileName(level));
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

            maps_[level] = writer_t::WriterNumBytes(getFileName(level),
                                                   size,
                                                   om::DONT_ZERO_FILL);
        }

        printf("memMappedVolume: done allocating data\n");
    }

    T* GetPtr(const int level) const {
        return maps_[level]->GetPtr();
    }

    T* GetChunkPtr(const om::chunkCoord& coord) const
    {
        const int level = coord.Level;
        const uint64_t offset =
            chunkOffset::ComputeChunkPtrOffsetBytes(vol_, coord);
        T* ret = maps_[level]->GetPtrWithOffset(offset);
        assert(ret);
        return ret;
    }

    int GetBytesPerVoxel() const {
        return sizeof(T);
    }

private:

    void resizeMapsVector(){
        maps_.resize(vol_->Coords().GetRootMipLevel() + 1);
    }

    std::string getFileName(const int level) const {
        return OmFileNames::GetMemMapFileName(vol_, level);
    }
};

