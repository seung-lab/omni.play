#pragma once

#include "common/common.h"
#include "mesh/io/dataForMeshLoad.hpp"
#include "mesh/iochunk/meshChunkDataReader.hpp"
#include "mesh/iomeshFilePtrCache.hpp"
#include "mesh/mesh::manager.h"

class meshReader{
private:
    segmentation *const segmentation_;
    const double threshold_;
    meshFilePtrCache *const filePtrCache_;

public:
    meshReader(mesh::manager* mesh::manager)
        : segmentation_(mesh::manager->GetSegmentation())
        , threshold_(mesh::manager->Threshold())
        , filePtrCache_(mesh::manager->FilePtrCache())
    {}

    ~meshReader()
    {}

    inline boost::shared_ptr<dataForMeshLoad>
    Read(const meshCoord& meshCoord){
        return Read(meshCoord.SegID(), meshCoord.Coord());
    }

    boost::shared_ptr<dataForMeshLoad>
    Read(const common::segId segID, const coords::chunkCoord& coord)
    {
        meshChunkAllocTableV2* chunk_table =
            filePtrCache_->GetAllocTable(coord);

        meshChunkDataReader chunk_data(segmentation_, coord, threshold_);

        boost::shared_ptr<dataForMeshLoad> ret =
            boost::make_shared<dataForMeshLoad>();

        if(!chunk_table->Contains(segID)){
            return ret;
        }

        const meshDataEntry entry = chunk_table->Find(segID);

        if(!entry.wasMeshed)
        {
            std::cout << "did not yet mesh " << segID
                      << " in coord " << coord << "\n" << std::flush;
            throw common::ioException("mesh data not found");
        }

        ret->HasData(entry.hasMeshData);

        if(!entry.hasMeshData){
            return ret;
        }

        ret->SetVertexIndex(chunk_data.Read<uint32_t>(entry.vertexIndex),
                            entry.vertexIndex.count,
                            entry.vertexIndex.totalBytes);

        ret->SetStripData(chunk_data.Read<uint32_t>(entry.stripData),
                          entry.stripData.count,
                          entry.stripData.totalBytes);

        ret->SetVertexData(chunk_data.Read<float>(entry.vertexData),
                           entry.vertexData.count,
                           entry.vertexData.totalBytes);

        if(entry.trianData.totalBytes){
            ret->SetTrianData(chunk_data.Read<uint32_t>(entry.trianData),
                              entry.trianData.count,
                              entry.trianData.totalBytes);
        }

        return ret;
    }
};

