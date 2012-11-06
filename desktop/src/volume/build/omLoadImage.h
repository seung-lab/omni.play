#pragma once

#include "common/common.h"
#include "common/omDebug.h"
#include "utility/omStringHelpers.h"
#include "datalayer/omDataWrapper.h"
#include "chunks/omChunk.h"
#include "volume/omMipVolume.h"
#include "utility/omTimer.hpp"
#include "threads/omTaskManager.hpp"
#include "threads/omTaskManagerTypes.h"

#include <QFile>
#include <QFileInfoList>
#include <QImage>

template <typename VOL, typename T>
class OmLoadImage {
private:
    VOL *const vol_;
    const boost::shared_ptr<QFile> mip0volFile_;
    const Vector3i mip0dims_;
    const std::vector<QFileInfo>& files_;
    const int totalNumImages_;
    const int totalTilesInSlice_;
    const uint64_t tileWidth_;
    const uint64_t tileHeight_;
    const uint64_t tilesPerChunk_;
    const uint64_t tileSizeBytes_;
    const uint64_t chunkSizeBytes_;
    QString msg_;

    LockedUint32 tileNum_;

    OmThreadPool taskMan_;

    const uint32_t numTilesToWrite_;
    zi::semaphore limit_;

    std::map<om::coords::Chunk, uint64_t> chunkOffsets_;

    bool shouldPreallocate_;

public:
    OmLoadImage(VOL* vol, boost::shared_ptr<QFile> mip0volFile,
                const std::vector<QFileInfo>& files)
        : vol_(vol)
        , mip0volFile_(mip0volFile)
        , mip0dims_(vol_->Coords().MipLevelDimensionsInMipChunks(0))
        , files_(files)
        , totalNumImages_(files.size())
        , totalTilesInSlice_(mip0dims_.x * mip0dims_.y)
        , tileWidth_(vol_->Coords().ChunkDimensions().x) // TODO: Fix assumptions about cubic chunks
        , tileHeight_(vol_->Coords().ChunkDimensions().x)
        , tilesPerChunk_(vol_->Coords().ChunkDimensions().x)
        , tileSizeBytes_(tileWidth_*tileHeight_*sizeof(T))
        , chunkSizeBytes_(tileSizeBytes_*tilesPerChunk_)
        , numTilesToWrite_(10000)
        , shouldPreallocate_(true)
    {
        tileNum_.set(0);
        limit_.set(numTilesToWrite_);
    }

    void Process()
    {
        taskMan_.start(1);

        for(size_t i = 0; i < files_.size(); ++i)
        {
            const QString fnp = files_[i].absoluteFilePath();
            processSlice(fnp, i);
        }

        std::cout << "\nwaiting for tile writes to complete..." << std::flush;

        limit_.acquire(numTilesToWrite_);

        taskMan_.join();

        mip0volFile_->flush();

        std::cout << "done\n";
    }

    void ReplaceSlice(const int sliceNum)
    {
        shouldPreallocate_ = false;

        taskMan_.start(1);

        processSlice(files_[0].absoluteFilePath(), sliceNum);

        std::cout << "\nwaiting for tile writes to complete..." << std::flush;

        limit_.acquire(numTilesToWrite_);

        taskMan_.join();

        std::cout << "done\n";
    }

private:
    void processSlice(const QString& fn, const int sliceNum )
    {
        msg_ = QString("(%1 of %2) loading image %3...")
            .arg(sliceNum+1)
            .arg(totalNumImages_)
            .arg(fn);
        printf("\n%s", qPrintable(fn));

        QImage img(fn);

        if(img.isNull())
        {
            printf("\tcould not read image data for %s; skipping...\n",
                   qPrintable(fn));
            return;
        }

        doProcessSlice(img, sliceNum);
    }

    uint64_t getChunkOffset(const om::coords::Chunk& coord)
    {
        if(chunkOffsets_.count(coord)){
            return chunkOffsets_[coord];
        }

        //if chunk was not in map, assume chunk is unallocated...
        //TODO: just use OmRawChunk...
        const uint64_t offset =
            OmChunkOffset::ComputeChunkPtrOffsetBytes(vol_, coord);

        if(shouldPreallocate_){
            preallocateChunk(offset);
        }

        return chunkOffsets_[coord] = offset;
    }

    void preallocateChunk(const uint64_t offset)
    {
        static const uint64_t pageSize = 4096;

        for(uint64_t i = 0; i < chunkSizeBytes_; i += pageSize)
        {
            mip0volFile_->seek(offset + i);
            mip0volFile_->putChar(0);
        }

        mip0volFile_->flush();
    }

    void doProcessSlice(const QImage& img, const int sliceNum)
    {
        const int z = sliceNum / tilesPerChunk_;

        for(int y = 0; y < mip0dims_.y; ++y)
        {
            for(int x = 0; x < mip0dims_.x; ++x)
            {
                const om::coords::Chunk coord = om::coords::Chunk(0, x, y, z);
                const om::coords::DataBbox chunk_bbox = coord.BoundingBox(*vol_);

                const int startX = chunk_bbox.getMin().x;
                const int startY = chunk_bbox.getMin().y;

                QImage tileQT = img.copy(startX, startY, tileWidth_, tileHeight_);
                uint8_t const*const tileDataQT = tileQT.bits();

                boost::shared_ptr<T> tile = OmSmartPtr<T>::MallocNumBytes(tileSizeBytes_,
                                                                       om::common::DONT_ZERO_FILL);

                std::copy(tileDataQT, tileDataQT + tileSizeBytes_, tile.get());

                limit_.acquire(1);

                taskMan_.push_back(
                    zi::run_fn(
                        zi::bind(&OmLoadImage<VOL,T>::tileWriterTask, this,
                                 tile, coord, sliceNum)));
            }
        }
    }

    void tileWriterTask(boost::shared_ptr<T> tile, const om::coords::Chunk coord, const int sliceNum)
    {
        const uint64_t chunkOffset = getChunkOffset(coord);

        const uint64_t sliceOffset = (sliceNum % tilesPerChunk_) * tileSizeBytes_;

        mip0volFile_->seek(chunkOffset+sliceOffset);

        mip0volFile_->write(reinterpret_cast<const char*>(tile.get()),
                            tileSizeBytes_);

        limit_.release(1);
    }
};

