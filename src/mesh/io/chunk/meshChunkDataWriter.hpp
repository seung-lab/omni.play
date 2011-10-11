#pragma once

#include "common/common.h"
#include "datalayer/fs/fileNames.hpp"
#include "mesh/iochunk/meshChunkDataWriterTask.hpp"
#include "utility/omLockedPODs.hpp"
#include "chunks/chunk.h"
#include "zi/omMutex.h"

class meshChunkDataWriter{
private:
    static const int defaultFileSizeMB = 2;
    static const int defaultFileExpansionFactor = 5;

private:
    segmentation *const vol_;
    const coords::chunkCoord& coord_;
    const double threshold_;
    const std::string fnp_;

    int64_t curEndOfFile_;

    zi::rwmutex lock_;

public:
    meshChunkDataWriter(segmentation* seg, const coords::chunkCoord& coord,
                            const double threshold)
        : vol_(seg)
        , coord_(coord)
        , threshold_(threshold)
        , fnp_(filePath())
        , curEndOfFile_(0)
    {
        openOrCreateFile();
    }

    ~meshChunkDataWriter(){
        shrinkFileIfNeeded();
    }

    template <typename T>
    void Append(const meshCoord meshCoord,
                std::vector<T>& data,
                meshFilePart& entry,
                const int64_t count)
    {
        const int64_t numBytes = sizeof(T) * data.size();

        entry.totalBytes = numBytes;
        entry.numElements = data.size();
        entry.count = count;

        expandFileIfNeeded(entry, numBytes);

        meshChunkDataWriterTask task(meshCoord,
                                         fnp_,
                                         entry.offsetIntoFile,
                                         numBytes);
        task.Write(data);
    }

    template <typename T>
    void Append(const meshCoord meshCoord,
                om::shared_ptr<T> data,
                meshFilePart& entry,
                const int64_t count,
                const int64_t numBytes)
    {
        if(0 != (numBytes % sizeof(T))){
            throw common::ioException("numBytes is not a multiple of type size");
        }

        const int64_t numElements = numBytes / sizeof(T);

        entry.totalBytes = numBytes;
        entry.numElements = numElements;
        entry.count = count;

        expandFileIfNeeded(entry, numBytes);

        meshChunkDataWriterTask task(meshCoord,
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
            throw common::ioException("could not open", fnp_);
        }

        if(fileDidNotExist){
            file.resize(defaultFileSizeMB * om::math::bytesPerMB);
        }

        curEndOfFile_ = file.size();
    }

    void expandFileIfNeeded(meshFilePart& entry, const int64_t numBytes)
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

    std::string filePath()
    {
        const std::string volPath = vol_->Folder()->GetMeshChunkFolderPath(threshold_, coord_);

        if(!QDir(volPath).exists()){
            vol_->Folder()->MakeMeshChunkFolderPath(threshold_, coord_);
        }

        return volPath + "meshData.ver2";
    }
};

