#pragma once

#include "volume/segmentationFolder.h"
#include "common/common.h"
#include "datalayer/fs/fileNames.hpp"
#include "chunks/chunk.h"
#include "mesh/iochunk/meshChunkTypes.h"

class meshChunkDataReader{
private:
    segmentation *const vol_;
    const coords::chunkCoord& coord_;
    const double threshold_;
    const std::string fnp_;

public:
    meshChunkDataReader(segmentation* seg, const coords::chunkCoord& coord,
                            const double threshold)
        : vol_(seg)
        , coord_(coord)
        , threshold_(threshold)
        , fnp_(filePath())
    {}

    ~meshChunkDataReader()
    {}

    // no locking needed
    template <typename T>
    om::shared_ptr<T> Read(const meshFilePart& entry)
    {
        const int64_t numBytes = entry.totalBytes;

        assert(numBytes);

        om::shared_ptr<T> ret =
            OmSmartPtr<T>::MallocNumBytes(numBytes, om::DONT_ZERO_FILL);

        char* dataCharPtr = reinterpret_cast<char*>(ret.get());

        QFile reader(fnp_);

        om::file::openFileRO(reader);

        if(!reader.seek(entry.offsetIntoFile))
        {
            throw common::ioException("could not seek to " +
                                om::string::num(entry.offsetIntoFile));
        }

        const int64_t bytesRead = reader.read(dataCharPtr, numBytes);

        if(bytesRead != numBytes)
        {
            std::cout << "could not read data; numBytes is " << numBytes
                      << ", but only read " << bytesRead << "\n"
                      << std::flush;
            throw common::ioException("could not read fully file", fnp_);
        }

        return ret;
    }

private:
    std::string filePath()
    {
        const std::string volPath = vol_->Folder()->GetMeshChunkFolderPath(threshold_, coord_);

        return volPath + "meshData.ver2";
    }
};

