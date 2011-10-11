#pragma once

#include "common/common.h"
#include "datalayer/fs/fileNames.hpp"
#include "volume/io/volumeData.h"
#include "volume/mipVolume.h"

#include <QFile>

template <typename T>
class rawChunkMemMapped {
private:
    mipVolume *const vol_;
    const coords::chunkCoord coord_;
    const uint64_t chunkOffset_;
    const std::string fnp_;
    const uint64_t numBytes_;

    boost::scoped_ptr<QFile> file_;
    T* dataRaw_;

public:
    rawChunkMemMapped(mipVolume* vol, const coords::chunkCoord& coord)
        : vol_(vol)
        , coord_(coord)
        , chunkOffset_(chunkOffset::ComputeChunkPtrOffsetBytes(vol, coord))
        , fnp_(fileNames::GetMemMapFileNameQT(vol, coord.Level))
        , numBytes_(128*128*128*vol_->GetBytesPerVoxel())
        , dataRaw_(NULL)
    {
        mapData();
    }

    ~rawChunkMemMapped()
    {}

    inline T Get(const uint64_t index) const {
        return dataRaw_[index];
    }

    uint64_t NumBytes() const {
        return numBytes_;
    }

    T* Data() const {
        return dataRaw_;
    }

private:
    void mapData()
    {
        file_.reset(new QFile(fnp_));

        if(!file_->open(QIODevice::ReadOnly)){
            throw common::ioException("could not open", fnp_);
        }

        uchar* map = file_->map(chunkOffset_, numBytes_);
        if(!map){
            throw common::ioException("could not map", fnp_);
        }

        dataRaw_ = reinterpret_cast<T*>(map);

        file_->close();
    }
};

