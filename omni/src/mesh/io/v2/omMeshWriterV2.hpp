#ifndef OM_MESH_IO_V2_HPP
#define OM_MESH_IO_V2_HPP

#include "common/omCommon.h"
#include "mesh/detail/TriStripCollector.hpp"
#include "mesh/io/omDataForMeshLoad.hpp"
#include "mesh/io/v2/chunk/omMeshChunkAllocTable.hpp"
#include "mesh/io/v2/chunk/omMeshChunkDataWriterV2.hpp"
#include "mesh/io/v2/threads/omMeshWriterTaskV2.hpp"
#include "mesh/io/v2/omMeshFilePtrCache.hpp"
#include "mesh/omMipMeshCoord.h"
#include "mesh/omMipMeshManager.h"

class OmMeshWriterV2{
private:
    OmSegmentation *const seg_;
    const double threshold_;
    OmMeshFilePtrCache* filePtrCache_;

public:
    OmMeshWriterV2(OmSegmentation* seg, const double threshold)
        : seg_(seg)
        , threshold_(threshold)
        , filePtrCache_(seg->MeshManager(threshold_)->FilePtrCache())
    {}

    ~OmMeshWriterV2(){
        Stop();
    }

    void Join(){
        filePtrCache_->Join();
    }

    void Stop(){
        filePtrCache_->Stop();
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
        boost::shared_ptr<OmMeshWriterTaskV2<U> > task =
            boost::make_shared<OmMeshWriterTaskV2<U> >(seg_,
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

#endif
