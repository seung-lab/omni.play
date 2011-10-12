#pragma once

#include "common/om.hpp"
#include "common/common.h"
#include "mesh/mesher/TriStripCollector.hpp"
#include "mesh/io/dataForMeshLoad.hpp"
#include "mesh/iochunk/meshChunkAllocTable.hpp"
#include "mesh/iochunk/meshChunkDataWriter.hpp"
#include "mesh/iomeshFilePtrCache.hpp"
#include "mesh/meshCoord.h"
#include "mesh/mesh::manager.h"
#include "zi/threads.h"

template <typename U>
class meshWriterTask : public zi::runnable{
private:
    segmentation *const seg_;
    meshFilePtrCache *const  filePtrCache_;
    const segId segID_;
    const coords::chunkCoord coord_;
    const U mesh_;
    const om::AllowOverwrite allowOverwrite_;

public:
    meshWriterTask(segmentation* seg,
                       meshFilePtrCache* filePtrCache,
                       const segId segID,
                       const coords::chunkCoord& coord,
                       const U mesh,
                       const om::AllowOverwrite allowOverwrite)
        : seg_(seg)
        , filePtrCache_(filePtrCache)
        , segID_(segID)
        , coord_(coord)
        , mesh_(mesh)
        , allowOverwrite_(allowOverwrite)
    {}

    void run()
    {
        meshChunkAllocTableV2* chunk_table =
            filePtrCache_->GetAllocTable(coord_);

        if(!chunk_table->Contains(segID_)){
            //printInfoAboutSkippedSegment(mesh_);
            return;
        }

        // prevent ovlapping mesh saves; will have segid "lock"
        //  if returns true
        if(!chunk_table->CanContinueMeshSave(segID_)){
            return;
        }

        if(om::WRITE_ONCE == allowOverwrite_)
        {
            const meshDataEntry entry = chunk_table->Find(segID_);
            if(entry.wasMeshed){
                chunk_table->SegmentMeshSaveDone(segID_);
                return;
            }
        }

        meshChunkDataWriter* chunk_data =
            filePtrCache_->GetWriter(coord_);

        const meshDataEntry entry =
            writeOutData(chunk_data, mesh_, meshCoord(coord_, segID_));

        chunk_table->Set(entry);

        chunk_table->SegmentMeshSaveDone(segID_);
    }

private:
    meshDataEntry
    writeOutData(meshChunkDataWriter* chunk_data,
                 om::shared_ptr<dataForMeshLoad> data,
                 const meshCoord& meshCoord)
    {
        meshDataEntry entry =
            om::meshio_::MakeEmptyEntry(meshCoord.SegID());

        entry.wasMeshed = true;

        // "empty mesh"
        if(0 == data->VertexDataCount()){
            return entry;
        }

        entry.hasMeshData = true;

        chunk_data->Append<uint32_t>(meshCoord,
                                     data->VertexIndexSharedPtr(),
                                     entry.vertexIndex,
                                     data->VertexIndexCount(),
                                     data->VertexIndexNumBytes());

        chunk_data->Append<float>(meshCoord,
                                  data->VertexDataSharedPtr(),
                                  entry.vertexData,
                                  data->VertexDataCount(),
                                  data->VertexDataNumBytes());

        chunk_data->Append<uint32_t>(meshCoord,
                                     data->StripDataSharedPtr(),
                                     entry.stripData,
                                     data->StripDataCount(),
                                     data->StripDataNumBytes());

        if(data->TrianDataNumBytes())
        {
            chunk_data->Append<uint32_t>(meshCoord,
                                         data->TrianDataSharedPtr(),
                                         entry.trianData,
                                         data->TrianDataCount(),
                                         data->TrianDataNumBytes());
        }

        return entry;
    }

    meshDataEntry
    writeOutData(meshChunkDataWriter* chunk_data,
                 TriStripCollector* triStrips,
                 const meshCoord& meshCoord)
    {
        std::vector<float>& data = triStrips->data_;
        std::vector<uint32_t>& indices = triStrips->indices_;
        std::vector<uint32_t>& strips = triStrips->strips_;

        meshDataEntry entry =
            om::meshio_::MakeEmptyEntry(meshCoord.SegID());
        entry.wasMeshed = true;

        // "empty mesh"
        if(0 == indices.size()){
            return entry;
        }

        entry.hasMeshData = true;

        chunk_data->Append<uint32_t>(meshCoord,
                                     indices,
                                     entry.vertexIndex,
                                     indices.size());

        chunk_data->Append<float>(meshCoord,
                                  data,
                                  entry.vertexData,
                                  data.size() / 6);

        chunk_data->Append<uint32_t>(meshCoord,
                                     strips,
                                     entry.stripData,
                                     strips.size() / 2);

        triStrips->clear();

        return entry;
    }

    void printInfoAboutSkippedSegment(om::shared_ptr<dataForMeshLoad>)
    {
        std::cout << "skipping segID " << segID_
                  << " in chunk " << coord_
                  << "\n";
    }

    void printInfoAboutSkippedSegment(zi::shared_ptr<TriStripCollector> triStrips)
    {
        printf(".");

        const int vertexData = triStrips->data_.size();
        const int vertexIndex = triStrips->indices_.size();
        const int stripsSize = triStrips->strips_.size();

        std::cout << "skipping segID " << segID_
                  << " in chunk " << coord_
                  << "; vertexData size (" << vertexData << ")"
                  << "; vertexIndex size (" << vertexIndex << ")"
                  << "; stripsSize size (" << stripsSize << ")"
                  << "\n";
    }
};

