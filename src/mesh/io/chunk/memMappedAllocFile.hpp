#pragma once

#include "mesh/io/chunk/meshChunkTypes.h"
#include "datalayer/fs/memMappedFile.hpp"

namespace om {
namespace mesh {

class memMappedAllocFile {
private:
    datalayer::memMappedFile<dataEntry> file_;
    dataEntry* table_;
    uint32_t numEntries_;

public:
    memMappedAllocFile(const std::string& fnp)
        : file_(fnp)
        , table_(file_.GetPtr())
        , numEntries_(file_.Size() / sizeof(dataEntry))
    {}

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
                          << "\n";
            }
        }

        return allGood;
    }

    dataEntry* Find(const dataEntry& entry)
    {
        if(!table_){
            return NULL;
        }

        dataEntry* iter = std::lower_bound(table_,
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

private:
    static bool compareBySegID(const dataEntry& a,
                               const dataEntry& b){
        return a.segID < b.segID;
    }
};

} // namespace mesh
} // namespace om
