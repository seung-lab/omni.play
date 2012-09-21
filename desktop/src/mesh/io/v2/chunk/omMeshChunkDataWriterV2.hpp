#pragma once

#include "common/omCommon.h"
#include "datalayer/fs/omFileNames.hpp"
#include "mesh/io/v2/chunk/omMeshChunkDataWriterTaskV2.hpp"
#include "utility/omLockedPODs.hpp"
#include "chunks/omChunk.h"
#include "zi/omMutex.h"

class OmMeshChunkDataWriterV2{
private:
    static const int defaultFileSizeMB = 2;
    static const int defaultFileExpansionFactor = 5;

private:
    OmSegmentation *const vol_;
    const om::chunkCoord& coord_;
    const double threshold_;
    const QString fnp_;

    int64_t curEndOfFile_;

    zi::rwmutex lock_;

public:
    OmMeshChunkDataWriterV2(OmSegmentation* seg, const om::chunkCoord& coord,
                            const double threshold)
        : vol_(seg)
        , coord_(coord)
        , threshold_(threshold)
        , fnp_(filePath())
        , curEndOfFile_(0)
    {
        openOrCreateFile();
    }

    ~OmMeshChunkDataWriterV2(){
        shrinkFileIfNeeded();
    }

    template <typename T>
    void Append(const OmMeshCoord meshCoord,
                std::vector<T>& data,
                OmMeshFilePart& entry,
                const int64_t count)
    {
        const int64_t numBytes = sizeof(T) * data.size();

        entry.totalBytes = numBytes;
        entry.numElements = data.size();
        entry.count = count;

        expandFileIfNeeded(entry, numBytes);

        OmMeshChunkDataWriterTaskV2 task(meshCoord,
                                         fnp_,
                                         entry.offsetIntoFile,
                                         numBytes);
        task.Write(data);
    }

    template <typename T>
    void Append(const OmMeshCoord meshCoord,
                om::shared_ptr<T> data,
                OmMeshFilePart& entry,
                const int64_t count,
                const int64_t numBytes)
    {
        if(0 != (numBytes % sizeof(T))){
            throw OmIoException("numBytes is not a multiple of type size");
        }

        const int64_t numElements = numBytes / sizeof(T);

        entry.totalBytes = numBytes;
        entry.numElements = numElements;
        entry.count = count;

        expandFileIfNeeded(entry, numBytes);

        OmMeshChunkDataWriterTaskV2 task(meshCoord,
                                         fnp_,
                                         entry.offsetIntoFile,
                                         numBytes);
        task.Write(data);
    }

private:
    void openOrCreateFile()
    {
        zi::rwmutex::write_guard g(lock_);

        QFile file(fnp_);
        const bool fileDidNotExist = !file.exists();

        if(!file.open(QIODevice::ReadWrite)){
            throw OmIoException("could not open", fnp_);
        }

        if(fileDidNotExist){
            file.resize(defaultFileSizeMB * om::math::bytesPerMB);
            curEndOfFile_ = 0;
        } else {
            curEndOfFile_ = file.size();
        }
    }

    void expandFileIfNeeded(OmMeshFilePart& entry, const int64_t numBytes)
    {
        zi::rwmutex::write_guard g(lock_);

        entry.offsetIntoFile = curEndOfFile_;
        curEndOfFile_ += numBytes;

        QFile file(fnp_);
        if(file.size() <= curEndOfFile_){
            file.resize(curEndOfFile_ * defaultFileExpansionFactor);
        }
    }

    void shrinkFileIfNeeded()
    {
        zi::rwmutex::write_guard g(lock_);

        QFile file(fnp_);
        if(file.size() > curEndOfFile_ ){
            file.resize(curEndOfFile_);
        }
    }

    QString filePath()
    {
        const QString volPath = vol_->Folder()->GetMeshChunkFolderPath(threshold_, coord_);

        if(!QDir(volPath).exists()){
            vol_->Folder()->MakeMeshChunkFolderPath(threshold_, coord_);
        }

        return volPath + "meshData.ver2";
    }
};

