#pragma once

#include "mesh/vertexIndex/chunkTypes.hpp"
#include "datalayer/readOnlyMemMappedFile.hpp"

namespace om {
namespace mesh {

class memMappedAllocFile {

 public:
  memMappedAllocFile(const file::path& fnp)
      : file_(fnp.string()),
        table_(file_.GetPtr()),
        numEntries_(file_.Size() / sizeof(DataEntry)) {}

  bool CheckEverythingWasMeshed() {
    bool allGood = true;

    for (auto i = 0; i < numEntries_; ++i) {
      if (!table_[i].wasMeshed) {
        allGood = false;

        log_debugs(io) << "missing mesh: "
                       << "segID " << table_[i].segID;
      }
    }

    return allGood;
  }

  const DataEntry* Find(const DataEntry& entry) {
    if (!table_) {
      return nullptr;
    }
    const DataEntry* iter =
        std::lower_bound(table_, table_ + numEntries_, entry, compareBySegID);
    if (iter == table_ + numEntries_ || iter->segID != entry.segID) {
      return nullptr;
    }

    return iter;
  }

 private:
  static bool compareBySegID(const DataEntry& a, const DataEntry& b) {
    return a.segID < b.segID;
  }

  datalayer::ReadOnlyMemMappedFile<DataEntry> file_;
  const DataEntry* table_;
  uint32_t numEntries_;
};

}  // namespace mesh
}  // namespace om
