#ifndef OM_CHUNK_UNIQUE_VALUES_PER_THRESHOLD_HPP
#define OM_CHUNK_UNIQUE_VALUES_PER_THRESHOLD_HPP

#include "common/om.hpp"
#include "chunks/uniqueValues/omChunkUniqueValuesTypes.h"
#include "datalayer/fs/omFileNames.hpp"
#include "utility/image/omImage.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "chunks/omSegChunk.h"
#include "chunks/omChunkCoord.h"
#include "volume/omSegmentation.h"

class OmChunkUniqueValuesPerThreshold {
private:
    OmSegmentation *const segmentation_;
    const OmChunkCoord coord_;
    const double threshold_;
    const QString fnp_;

    om::shared_ptr<uint32_t> values_;
    size_t numElements_;

    zi::rwmutex mutex_;

public:
    OmChunkUniqueValuesPerThreshold(OmSegmentation* segmentation,
                                    const OmChunkCoord& coord,
                                    const double threshold)
        : segmentation_(segmentation)
        , coord_(coord)
        , threshold_(threshold)
        , fnp_(filePath())
        , numElements_(0)
    {}

    ChunkUniqueValues Values()
    {
        zi::rwmutex::write_guard g(mutex_);

        if(!values_){
            load();
        }

        return ChunkUniqueValues(values_, numElements_);
    }

    void RereadChunk()
    {
        zi::rwmutex::write_guard g(mutex_);

        findValues();
    }

private:
    void load()
    {
        QFile file(fnp_);
        if(!file.exists()){
            findValues();
            return;
        }

        if(!file.open(QIODevice::ReadOnly)) {
            throw OmIoException("could not open", fnp_);
        }

        values_ = OmSmartPtr<uint32_t>::MallocNumBytes(file.size(),
                                                       om::DONT_ZERO_FILL);
        numElements_ = file.size() / sizeof(uint32_t);

        file.seek(0);

        char* data = reinterpret_cast<char*>(values_.get());
        file.read(data, file.size());
    }

    void findValues()
    {
        OmSegChunk* chunk = segmentation_->GetChunk(coord_);

        om::shared_ptr<uint32_t> rawDataPtr =
            chunk->SegData()->GetCopyOfChunkDataAsUint32();

        uint32_t const*const rawData = rawDataPtr.get();

        boost::unordered_set<uint32_t> segIDs;

        if(!qFuzzyCompare(1, threshold_))
        {
            OmSegments* segments = segmentation_->Segments();
            segmentation_->SetDendThreshold(threshold_);

            for(size_t i = 0; i < chunk->Mipping().NumVoxels(); ++i){
                if( 0 != rawData[i]) {
                    segIDs.insert(segments->findRootID(rawData[i]));
                }
            }
        } else {
            for(size_t i = 0; i < chunk->Mipping().NumVoxels(); ++i){
                if( 0 != rawData[i]) {
                    segIDs.insert(rawData[i]);
                }
            }
        }

        values_ = OmSmartPtr<uint32_t>::MallocNumElements(segIDs.size(),
                                                          om::DONT_ZERO_FILL);

        std::copy(segIDs.begin(), segIDs.end(), values_.get());
        zi::sort(values_.get(), values_.get() + segIDs.size());

        numElements_ = segIDs.size();

        store();
    }

    void store()
    {
        QFile file(fnp_);

        if(!file.open(QIODevice::WriteOnly)) {
            throw OmIoException("could not open", fnp_);
        }

        const int64_t numBytes = numElements_ * sizeof(uint32_t);
        file.resize(numBytes);

        const char* data = reinterpret_cast<const char*>(values_.get());

        file.write(data, numBytes);
    }

    QString filePath()
    {
        const QString volPath =
            OmFileNames::GetChunkFolderPath(segmentation_, coord_);

        if(!QDir(volPath).exists()){
            OmFileNames::MakeChunkFolderPath(segmentation_, coord_);
        }

        const QString fullPath = QString("%1uniqeValues.%2.ver1")
            .arg(volPath)
            .arg(QString::number(threshold_, 'f', 4));

        return fullPath;
    }
};

#endif
