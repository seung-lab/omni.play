#pragma once

#include "common/common.h"
#include "mesh/io/dataForMeshLoad.hpp"
#include "mesh/iochunk/meshChunkAllocTable.hpp"
#include "mesh/iochunk/meshChunkDataWriter.hpp"
#include "mesh/iomeshFilePtrCache.hpp"
#include "mesh/iothreads/meshWriterTask.hpp"
#include "mesh/mesher/TriStripCollector.hpp"
#include "mesh/meshCoord.h"
#include "mesh/meshManager.h"

class meshWriter{
private:
    segmentation *const segmentation_;
    const double threshold_;
    meshFilePtrCache* filePtrCache_;

public:
    meshWriter(meshManager* meshManager)
        : segmentation_(meshManager->GetSegmentation())
        , threshold_(meshManager->Threshold())
        , filePtrCache_(meshManager->FilePtrCache())
    {}

    ~meshWriter()
    {
        Join();
        filePtrCache_->FlushMappedFiles();
    }

    void Join(){
        filePtrCache_->Join();
    }

    bool CheckEverythingWasMeshed()
    {
        om::shared_ptr<std::deque<coords::chunkCoord> > coordsPtr =
            segmentation_->GetMipChunkCoords();

        bool allGood = true;

        std::cout << "\nchecking that all segments were meshed...\n";

        FOR_EACH(iter, *coordsPtr)
        {
            meshChunkAllocTableV2* chunk_table =
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

    bool Contains(const segId segID, const coords::chunkCoord& coord)
    {
        meshChunkAllocTableV2* chunk_table =
            filePtrCache_->GetAllocTable(coord);
        return chunk_table->Contains(segID);
    }

    bool WasMeshed(const segId segID, const coords::chunkCoord& coord)
    {
        meshChunkAllocTableV2* chunk_table =
            filePtrCache_->GetAllocTable(coord);

        if(!chunk_table->Contains(segID)){
            throw OmIoException("segID not present");
        }

        const meshDataEntry entry = chunk_table->Find(segID);

        return entry.wasMeshed;
    }

    bool HasData(const segId segID, const coords::chunkCoord& coord)
    {
        meshChunkAllocTableV2* chunk_table =
            filePtrCache_->GetAllocTable(coord);

        if(!chunk_table->Contains(segID)){
            throw OmIoException("segID not present");
        }

        const meshDataEntry entry = chunk_table->Find(segID);

        if(!entry.wasMeshed){
            throw OmIoException("was not yet meshed");
        }

        return entry.hasMeshData;
    }

    // Save will take ownership of mesh data
    template <typename U>
    void Save(const segId segID, const coords::chunkCoord& coord,
              const U data, const om::ShouldBufferWrites buffferWrites,
              const om::AllowOverwrite allowOverwrite)
    {
        om::shared_ptr<meshWriterTask<U> > task =
            om::make_shared<meshWriterTask<U> >(segmentation_,
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

