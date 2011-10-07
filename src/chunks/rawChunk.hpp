#pragma once

#include "common/common.h"
#include "datalayer/fs/fileNames.hpp"
#include "utility/omSmartPtr.hpp"
#include "volume/io/volumeData.h"
#include "volume/mipVolume.h"
#include "volume/volumeTypes.h"
#include "zi/omMutex.h"
#include "zi/omThreads.h"

#include <QFile>

template <typename T>
class rawChunk {
private:
    mipVolume *const vol_;
    const coords::chunkCoord coord_;
    const uint64_t chunkOffset_;
    const std::string memMapFileName_;
    const uint64_t numBytes_;

    om::shared_ptr<T> data_;
    T* dataRaw_;

    bool dirty_;

    struct raw_chunk_mutex_pool_tag;
    typedef typename zi::spinlock::pool<raw_chunk_mutex_pool_tag>::guard mutex_guard_t;

public:
    rawChunk(mipVolume* vol, const coords::chunkCoord& coord)
        : vol_(vol)
        , coord_(coord)
        , chunkOffset_(chunkOffset::ComputeChunkPtrOffsetBytes(vol, coord))
        , memMapFileName_(fileNames::GetMemMapFileNameQT(vol,
                                                           coord.Level))
        , numBytes_(128*128*128*vol_->GetBytesPerVoxel())
        , dataRaw_(NULL)
        , dirty_(false)
    {
        readData();
    }

    ~rawChunk()
    {
        Flush();
    }

    void SetDirty(){
        dirty_ = true;
    }

    void Flush()
    {
        if(dirty_)
        {
            std::cout << "flushing " << coord_ << "\n";
            writeData();
            dirty_ = false;
        }
    }

    inline void Set(const uint64_t index, const T val)
    {
        dirty_ = true;
        dataRaw_[index] = val;
    }

    inline T Get(const uint64_t index) const
    {
        return dataRaw_[index];
    }

    uint64_t NumBytes() const {
        return numBytes_;
    }

    T* Data(){
        return dataRaw_;
    }

    om::shared_ptr<T> SharedPtr(){
        return data_;
    }

private:
    void readData()
    {
        mutex_guard_t g(chunkOffset_); // prevent read-while-writing errors

        QFile file(memMapFileName_);
        if(!file.open(QIODevice::ReadOnly)){
            throw OmIoException("could not open", memMapFileName_);
        }

        file.seek(chunkOffset_);

        data_ = OmSmartPtr<T>::MallocNumBytes(numBytes_, om::DONT_ZERO_FILL);
        char* dataAsCharPtr = (char*)(data_.get());
        const uint64_t readSize = file.read(dataAsCharPtr, numBytes_);

        if( readSize != numBytes_) {
            throw OmIoException("read failed");
        }

        dataRaw_ = data_.get();
    }

    void writeData()
    {
        mutex_guard_t g(chunkOffset_); // prevent read-while-writing errors

        QFile file(memMapFileName_);
        if(!file.open(QIODevice::ReadWrite)){
            throw OmIoException("could not open", memMapFileName_);
        }

        file.seek(chunkOffset_);
        const uint64_t writeSize =
            file.write(reinterpret_cast<const char*>(dataRaw_), numBytes_);

        if( writeSize != numBytes_) {
            throw OmIoException("write failed");
        }
    }
};

