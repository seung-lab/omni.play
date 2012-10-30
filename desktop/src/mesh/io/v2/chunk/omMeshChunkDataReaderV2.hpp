#pragma once

#include "volume/omSegmentationFolder.h"
#include "common/common.h"
#include "datalayer/fs/omFileNames.hpp"
#include "chunks/omChunk.h"
#include "mesh/io/v2/chunk/omMeshChunkTypes.h"

class OmMeshChunkDataReaderV2{
private:
    OmSegmentation *const vol_;
    const om::coords::Chunk& coord_;
    const double threshold_;
    const QString fnp_;

public:
    OmMeshChunkDataReaderV2(OmSegmentation* seg, const om::coords::Chunk& coord,
                            const double threshold)
        : vol_(seg)
        , coord_(coord)
        , threshold_(threshold)
        , fnp_(filePath())
    {}

    ~OmMeshChunkDataReaderV2()
    {}

    // no locking needed
    template <typename T>
    om::shared_ptr<T> Read(const OmMeshFilePart& entry)
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
            throw OmIoException("could not seek to " +
                                om::string::num(entry.offsetIntoFile));
        }

        const int64_t bytesRead = reader.read(dataCharPtr, numBytes);

        if(bytesRead != numBytes)
        {
            std::cout << "could not read data; numBytes is " << numBytes
                      << ", but only read " << bytesRead << "\n"
                      << std::flush;
            throw OmIoException("could not read fully file", fnp_);
        }

        return ret;
    }

private:
    QString filePath()
    {
        const QString volPath = vol_->Folder()->GetMeshChunkFolderPath(threshold_, coord_);

        return volPath + "meshData.ver2";
    }
};

