#pragma once

#include "common/common.h"
#include "mesh/dataForMeshLoad.hpp"
#include "mesh/meshChunkDataReader.hpp"
#include "mesh/dataForMeshLoad.hpp"
#include "mesh/memMappedAllocFile.hpp"

namespace om {
namespace mesh {

class reader{
private:
    std::string fnp_;

public:
    reader(std::string fnp)
        : fnp_(fnp)
    {}

    ~reader()
    {}

    boost::shared_ptr<data>
    Read(const common::segId segID, const coords::chunk& coord)
    {
        std::stringstream ss;
        ss << fnp_ << "meshAllocTable.ver2";
        std::string allocTablePath;
        ss >> allocTablePath;
        if(!file::exists(allocTablePath)) {
            throw argException("Alloc table file not found.");
        }

        std::stringstream ss2;
        ss2 << fnp_ << "meshData.ver2";
        std::string dataPath;
        ss2 >> dataPath;
        if(!file::exists(dataPath)) {
            throw argException("Mesh data file not found.");
        }

        memMappedAllocFile chunk_table(allocTablePath);
        chunkDataReader chunk_data(dataPath);

        boost::shared_ptr<data> ret = boost::make_shared<data>();

        const dataEntry* entry = chunk_table.Find(MakeEmptyEntry(segID));

        if(!entry || !entry->wasMeshed)
        {
            std::cout << "did not yet mesh " << segID
                      << " in coord " << coord << "\n" << std::flush;
            throw ioException("mesh data not found");
        }

        ret->HasData(entry->hasMeshData);

        if(!entry->hasMeshData){
            return ret;
        }

        ret->SetVertexIndex(chunk_data.Read<uint32_t>(entry->vertexIndex),
                            entry->vertexIndex.count,
                            entry->vertexIndex.totalBytes);

        ret->SetStripData(chunk_data.Read<uint32_t>(entry->stripData),
                          entry->stripData.count,
                          entry->stripData.totalBytes);

        ret->SetVertexData(chunk_data.Read<float>(entry->vertexData),
                           entry->vertexData.count,
                           entry->vertexData.totalBytes);

        if(entry->trianData.totalBytes){
            ret->SetTrianData(chunk_data.Read<uint32_t>(entry->trianData),
                              entry->trianData.count,
                              entry->trianData.totalBytes);
        }

        return ret;
    }
};

} // namespace mesh
} // namespace om
