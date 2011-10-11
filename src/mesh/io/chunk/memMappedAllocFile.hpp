#pragma once

#include "chunks/chunk.h"
#include "chunks/uniqueValues/chunkUniqueValuesManager.hpp"
#include "mesh/iochunk/meshChunkTypes.h"
#include "utility/omStringHelpers.h"
#include "volume/segmentation.h"

class memMappedAllocFile {
private:
    segmentation *const segmentation_;
    const coords::chunkCoord coord_;
    const double threshold_;
    const std::string fnp_;

    boost::scoped_ptr<QFile> file_;
    meshDataEntry* table_;
    uint32_t numEntries_;

public:
    memMappedAllocFile(segmentation* segmentation,
                         const coords::chunkCoord& coord,
                         const double threshold)
        : segmentation_(segmentation)
        , coord_(coord)
        , threshold_(threshold)
        , fnp_(filePath())
        , table_(NULL)
        , numEntries_(0)
    {}

    bool CreateIfNeeded()
    {
        QFile file(fnp_);
        if(file.exists()){
            return false;
        }

        setupFile();
        return true;
    }

    bool CheckEverythingWasMeshed()
    {
        bool allGood = true;

        for(uint32_t i = 0; i < numEntries_; ++i)
        {
            if(!table_[i].wasMeshed)
            {
                allGood = false;

                std::cout << "missing mesh: "
                          << "segID " << table_[i].segID
                          << " in coord " << coord_
                          << "\n";
            }
        }

        return allGood;
    }

    meshDataEntry* Find(const meshDataEntry& entry)
    {
        if(!table_){
            return NULL;
        }

        meshDataEntry* iter =
            std::lower_bound(table_,
                             table_ + numEntries_,
                             entry,
                             compareBySegID);

        if(iter == table_ + numEntries_ ||
           iter->segID != entry.segID)
        {
            return NULL;
        }

        return iter;
    }

    void Unmap()
    {
        file_.reset();
        table_ = NULL;
        numEntries_ = 0;
    }

    inline bool MapIfNeeded()
    {
        if(file_){
            return false;
        }

        map();

        return true;
    }

private:

    void map()
    {
        file_.reset(new QFile(fnp_));

        if(!file_->exists()){
            return;
        }

        if(!file_->open(QIODevice::ReadWrite)) {
            throw common::ioException("could not open", fnp_);
        }

        uchar* map = file_->map(0, file_->size());
        if(!map){
            throw common::ioException("could not map", fnp_);
        }

        file_->close();

        table_ = reinterpret_cast<meshDataEntry*>(map);
        numEntries_ = file_->size() / sizeof(meshDataEntry);
    }

    void setupFile()
    {
        const ChunkUniqueValues segIDs =
            segmentation_->ChunkUniqueValues()->Values(coord_, threshold_);

        if(!segIDs.size()){
            return;
        }

        file_.reset(new QFile(fnp_));

        if(!file_->open(QIODevice::ReadWrite)) {
            throw common::ioException("could not open", fnp_);
        }

        file_->resize(segIDs.size() * sizeof(meshDataEntry));

        uchar* map = file_->map(0, file_->size());
        if(!map){
            throw common::ioException("could not map", fnp_);
        }

        file_->close();

        table_ = reinterpret_cast<meshDataEntry*>(map);
        numEntries_ = segIDs.size();

        resetTable(segIDs);
        sortTable();

        // std::cout << "in chunk " << coord_
        //           << ", found "
        //           << om::string::humanizeNum(numEntries_)
        //           << " segment IDs\n";
    }

    struct ResetEntry {
        meshDataEntry operator()(const segId segID) const {
            return om::meshio_::MakeEmptyEntry(segID);
        }
    };

    void resetTable(const ChunkUniqueValues& segIDs)
    {
        assert(table_);

        zi::transform(segIDs.begin(),
                      segIDs.end(),
                      table_,
                      ResetEntry());
    }

    void sortTable()
    {
        assert(table_);

        zi::sort(table_,
                 table_ + numEntries_,
                 compareBySegID);
    }

    static bool compareBySegID(const meshDataEntry& a,
                               const meshDataEntry& b){
        return a.segID < b.segID;
    }

    std::string filePath()
    {
        const std::string volPath = segmentation_->Folder()->GetMeshChunkFolderPath(threshold_, coord_);

        if(!QDir(volPath).exists()){
            segmentation_->Folder()->MakeMeshChunkFolderPath(threshold_, coord_);
        }

        const std::string fullPath = std::string("%1meshAllocTable.ver2")
            .arg(volPath);

        return fullPath;
    }
};

