#pragma once

#include "common/omCommon.h"
#include "mesh/io/omDataForMeshLoad.hpp"
#include "mesh/io/v2/chunk/omMeshChunkAllocTable.hpp"
#include "mesh/io/v2/chunk/omMeshChunkDataWriterV2.hpp"
#include "mesh/io/v2/omMeshFilePtrCache.hpp"
#include "mesh/io/v2/threads/omMeshWriterTaskV2.hpp"
#include "mesh/mesher/TriStripCollector.hpp"
#include "mesh/omMeshCoord.h"
#include "mesh/omMeshManager.h"

class OmMeshWriterV2{
private:
    OmSegmentation *const segmentation_;
    const double threshold_;
    OmMeshFilePtrCache* filePtrCache_;

public:
    OmMeshWriterV2(OmMeshManager* meshManager)
        : segmentation_(meshManager->GetSegmentation())
        , threshold_(meshManager->Threshold())
        , filePtrCache_(meshManager->FilePtrCache())
    {}

    ~OmMeshWriterV2()
    {
        Join();
        filePtrCache_->FlushMappedFiles();
    }

    void Join(){
        filePtrCache_->Join();
    }

    bool CheckEverythingWasMeshed()
    {
        om::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
            segmentation_->GetMipChunkCoords();

        bool allGood = true;

        std::cout << "\nchecking that all segments were meshed...\n";

        FOR_EACH(iter, *coordsPtr)
        {
            OmMeshChunkAllocTableV2* chunk_table =
                filePtrCache_->GetAllocTable(*iter);

            if(!chunk_table->CheckEverythingWasMeshed()){
                allGood = false;
            }
        }

        if(allGood){
            std::cout << "all segments meshed!\n";

        } else {
            std::cout << "\nERROR: some segments not meshed!\n";
            throw OmIoException("some segments not meshed");
        }

        return allGood;
    }

    bool Contains(const OmSegID segID, const OmChunkCoord& coord)
    {
        OmMeshChunkAllocTableV2* chunk_table =
            filePtrCache_->GetAllocTable(coord);
        return chunk_table->Contains(segID);
    }

    bool WasMeshed(const OmSegID segID, const OmChunkCoord& coord)
    {
        OmMeshChunkAllocTableV2* chunk_table =
            filePtrCache_->GetAllocTable(coord);

        if(!chunk_table->Contains(segID)){
            throw OmIoException("segID not present");
        }

        const OmMeshDataEntry entry = chunk_table->Find(segID);

        return entry.wasMeshed;
    }

    bool HasData(const OmSegID segID, const OmChunkCoord& coord)
    {
        OmMeshChunkAllocTableV2* chunk_table =
            filePtrCache_->GetAllocTable(coord);

        if(!chunk_table->Contains(segID)){
            throw OmIoException("segID not present");
        }

        const OmMeshDataEntry entry = chunk_table->Find(segID);

        if(!entry.wasMeshed){
            throw OmIoException("was not yet meshed");
        }

        return entry.hasMeshData;
    }

    // Save will take ownership of mesh data
    template <typename U>
    void Save(const OmSegID segID, const OmChunkCoord& coord,
              const U data, const om::ShouldBufferWrites buffferWrites,
              const om::AllowOverwrite allowOverwrite)
    {
        om::shared_ptr<OmMeshWriterTaskV2<U> > task =
            om::make_shared<OmMeshWriterTaskV2<U> >(segmentation_,
                                                       filePtrCache_,
                                                       segID,
                                                       coord,
                                                       data,
                                                       allowOverwrite);

        static const uint32_t maxNumberTasks = 500;
        const uint32_t curNumberTasks = filePtrCache_->NumTasks();
        if(curNumberTasks > maxNumberTasks){
            std::cout << "write back queue size " << curNumberTasks << "\n";
        }

        if(om::BUFFER_WRITES == buffferWrites &&
           curNumberTasks < maxNumberTasks)
        {
            filePtrCache_->AddTaskBack(task);

        } else {
            task->run();
        }
    }
};

