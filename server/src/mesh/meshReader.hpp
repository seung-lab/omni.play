#pragma once

#include "common/common.h"
#include "mesh/dataForMeshLoad.hpp"
#include "mesh/meshChunkDataReader.hpp"
#include "mesh/dataForMeshLoad.hpp"
#include "mesh/memMappedAllocFile.hpp"
#include <boost/format.hpp>

namespace om {
namespace mesh {

class reader{
private:
    boost::scoped_ptr<memMappedAllocFile> chunkTable_;
    boost::scoped_ptr<chunkDataReader> chunkData_;

public:
    reader(std::string fnp, const coords::chunk& coord)
    {
        std::string path = str(boost::format("%1%/1.0000/%2%/%3%/%4%/%5%/")
                               % fnp % coord.getLevel() % coord.X() % coord.Y() % coord.Z());

        std::stringstream ss;
        ss << path << "meshAllocTable.ver2";
        std::string allocTablePath;
        ss >> allocTablePath;
        if(!file::exists(allocTablePath)) {
            throw argException("Alloc table file not found.");
        }

        chunkTable_.reset(new memMappedAllocFile(allocTablePath));

        std::stringstream ss2;
        ss2 << path << "meshData.ver2";
        std::string dataPath;
        ss2 >> dataPath;
        if(!file::exists(dataPath)) {
            throw argException("Mesh data file not found.");
        }

        chunkData_.reset(new chunkDataReader(dataPath));
    }

    ~reader()
    {}

    dataEntry* GetDataEntry(const common::segId segId)
    {
        return chunkTable_->Find(MakeEmptyEntry(segId));
    }

    boost::shared_ptr<data>
    Read(const common::segId segId)
    {
        boost::shared_ptr<data> ret = boost::make_shared<data>();

        const dataEntry* entry = GetDataEntry(segId);

        std::cout << entry->segID << ", "
                  << entry->wasMeshed << ", "
                  << entry->hasMeshData;


        if(!entry || !entry->wasMeshed)
        {
            return ret;
        }

        if(!entry->hasMeshData){
            return ret;
        } else {
            ret->HasData(true);
        }

        ret->SetVertexIndex(chunkData_->Read<uint32_t>(entry->vertexIndex),
                            entry->vertexIndex.count,
                            entry->vertexIndex.totalBytes);

        ret->SetStripData(chunkData_->Read<uint32_t>(entry->stripData),
                          entry->stripData.count,
                          entry->stripData.totalBytes);

        ret->SetVertexData(chunkData_->Read<float>(entry->vertexData),
                           entry->vertexData.count,
                           entry->vertexData.totalBytes);

        if(entry->trianData.totalBytes){
            ret->SetTrianData(chunkData_->Read<uint32_t>(entry->trianData),
                              entry->trianData.count,
                              entry->trianData.totalBytes);
        }

        return ret;
    }
};

} // namespace mesh
} // namespace om
