#pragma once

#include "chunks/omChunkCoord.h"
#include "datalayer/fs/omFileNames.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omMipVolume.h"

#include <QFile>

template <typename T>
class OmRawChunkMemMapped {
private:
    OmMipVolume *const vol_;
    const OmChunkCoord coord_;
    const uint64_t chunkOffset_;
    const QString fnp_;
    const uint64_t numBytes_;

    boost::scoped_ptr<QFile> file_;
    T* dataRaw_;

public:
    OmRawChunkMemMapped(OmMipVolume* vol, const OmChunkCoord& coord)
        : vol_(vol)
        , coord_(coord)
        , chunkOffset_(OmChunkOffset::ComputeChunkPtrOffsetBytes(vol, coord))
        , fnp_(OmFileNames::GetMemMapFileNameQT(vol, coord.Level))
        , numBytes_(128*128*128*vol_->GetBytesPerVoxel())
        , dataRaw_(NULL)
    {
        mapData();
    }

    ~OmRawChunkMemMapped()
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
            throw OmIoException("could not open", fnp_);
        }

        uchar* map = file_->map(chunkOffset_, numBytes_);
        if(!map){
            throw OmIoException("could not map", fnp_);
        }

        dataRaw_ = reinterpret_cast<T*>(map);

        file_->close();
    }
};

