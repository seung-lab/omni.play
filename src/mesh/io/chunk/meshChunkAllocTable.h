#pragma once

#include "datalayer/fs/fileNames.hpp"
#include "mesh/iochunk/memMappedAllocFile.hpp"
#include "utility/omLockedObjects.h"

class meshFilePtrCache;

class meshChunkAllocTableV2 {
private:
    meshFilePtrCache *const filePtrCache_;
    boost::scoped_ptr<memMappedAllocFile> file_;

    LockedSet<common::segId> segsBeingSaved_;
    zi::rwmutex lock_;

public:
    meshChunkAllocTableV2(meshFilePtrCache* filePtrCache,
                            segmentation* seg,
                            const coords::chunkCoord& coord,
                            const double threshold)
        : filePtrCache_(filePtrCache)
        , file_(new memMappedAllocFile(seg, coord, threshold))
    {
        zi::rwmutex::write_guard g(lock_);

        if(file_->CreateIfNeeded()){
            registerMappedFile();
        }
    }

    ~meshChunkAllocTableV2()
    {}

    // avoid over-lapping writes for meshes with same segment ID
    bool CanContinueMeshSave(const common::segId segID)
    {
        if(segsBeingSaved_.insertSinceWasntPresent(segID)){
            return true;
        }
        printf("mesh already being saved...\n");
        return false;
    }

    void SegmentMeshSaveDone(const common::segId segID){
        segsBeingSaved_.erase(segID);
    }

    void Unmap()
    {
        zi::rwmutex::write_guard g(lock_);
        file_->Unmap();
    }

    bool Contains(const meshDataEntry& newEntry)
    {
        zi::rwmutex::write_guard g(lock_);

        map();

        meshDataEntry* entry = file_->Find(newEntry);
        if(!entry){
            return false;
        }
        return true;
    }

    inline bool Contains(const common::segId segID)
    {
        meshDataEntry e;
        e.segID = segID;
        return Contains(e);
    }

    void Set(const meshDataEntry& newEntry)
    {
        zi::rwmutex::write_guard g(lock_);

        map();

        meshDataEntry* entry = file_->Find(newEntry);
        if(!entry){
            throw common::ioException("unknown segment ID");
        }

        assert(newEntry.segID == entry->segID);

        (*entry) = newEntry;
    }

    const meshDataEntry Find(const common::segId segID)
    {
        zi::rwmutex::write_guard g(lock_);

        map();

        meshDataEntry e;
        e.segID = segID;

        meshDataEntry* entry = file_->Find(e);
        if(!entry){
            throw common::ioException("unknown segment ID");
        }

        return *entry;
    }

    bool CheckEverythingWasMeshed()
    {
        zi::rwmutex::write_guard g(lock_);

        map();

        return file_->CheckEverythingWasMeshed();
    }

private:
    inline void map()
    {
        if(file_->MapIfNeeded()){
            registerMappedFile();
        }
    }

    void registerMappedFile();
};

