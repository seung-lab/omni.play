#pragma once

#include "common/common.h"
#include "datalayer/fs/fileNames.hpp"
#include "utility/smartPtr.hpp"
#include "volume/io/volumeData.h"
#include "volume/io/chunkOffset.hpp"
#include "volume/volume.h"
#include "volume/volumeTypes.h"
#include "zi/mutex.h"
#include "zi/threads.h"

#include <fstream>

namespace om {
namespace chunks {

template <typename T>
class rawChunk {
private:
    volume::volume *const vol_;
    const coords::chunkCoord coord_;
    const uint64_t chunkOffset_;
    const std::string memMapFileName_;
    const uint64_t numBytes_;

    boost::shared_ptr<T> data_;

    T* dataRaw_;

    bool dirty_;

    struct raw_chunk_mutex_pool_tag;
    typedef typename zi::spinlock::pool<raw_chunk_mutex_pool_tag>::guard mutex_guard_t;

public:
    rawChunk(volume::volume* vol, const coords::chunkCoord& coord)
        : vol_(vol)
        , coord_(coord)
        , chunkOffset_(volume::ComputeChunkPtrOffsetBytes(vol, coord))
        , memMapFileName_(datalayer::fileNames::GetMemMapFileName(vol,
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

    boost::shared_ptr<T> SharedPtr(){
        return data_;
    }

private:
    void readData()
    {
        mutex_guard_t g(chunkOffset_); // prevent read-while-writing errors

        std::fstream file(memMapFileName_, std::ios_base::in | std::ios_base::binary);
        if(!file.is_open()){
            throw common::ioException("could not open", memMapFileName_);
        }

        file.seekg(chunkOffset_);

        data_ = utility::smartPtr<T>::MallocNumBytes(numBytes_, common::DONT_ZERO_FILL);
        char* dataAsCharPtr = (char*)(data_.get());
        file.read(dataAsCharPtr, numBytes_);

        if( file.gcount() != numBytes_) {
            throw common::ioException("read failed");
        }

        dataRaw_ = data_.get();
    }

    void writeData()
    {
        mutex_guard_t g(chunkOffset_); // prevent read-while-writing errors

        std::fstream file(memMapFileName_, std::ios_base::out | std::ios_base::binary);
        if(!file.is_open()){
            throw common::ioException("could not open", memMapFileName_);
        }

        file.seekp(chunkOffset_);
        file.write(reinterpret_cast<const char*>(dataRaw_), numBytes_);
    }
};

}
}
