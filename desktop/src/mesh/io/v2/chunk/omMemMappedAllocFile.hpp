#pragma once
#include "precomp.h"

#include "chunks/omChunk.h"
#include "chunks/uniqueValues/omChunkUniqueValuesManager.hpp"
#include "mesh/io/v2/chunk/omMeshChunkTypes.h"
#include "utility/omStringHelpers.h"
#include "volume/omSegmentation.h"

class OmMemMappedAllocFile {
 private:
  OmSegmentation* const segmentation_;
  const om::coords::Chunk coord_;
  const double threshold_;
  const QString fnp_;

  std::unique_ptr<QFile> file_;
  OmMeshDataEntry* table_;
  uint32_t numEntries_;

 public:
  OmMemMappedAllocFile(OmSegmentation* segmentation,
                       const om::coords::Chunk& coord, const double threshold)
      : segmentation_(segmentation),
        coord_(coord),
        threshold_(threshold),
        fnp_(filePath()),
        table_(nullptr),
        numEntries_(0) {}

  bool CreateIfNeeded() {
    QFile file(fnp_);
    if (file.exists()) {
      return false;
    }

    setupFile();
    return true;
  }

  bool CheckEverythingWasMeshed() {
    bool allGood = true;

    for (uint32_t i = 0; i < numEntries_; ++i) {
      if (!table_[i].wasMeshed) {
        allGood = false;

        log_infos << "missing mesh: "
                  << "segID " << table_[i].segID << " in coord " << coord_;
      }
    }

    return allGood;
  }

  OmMeshDataEntry* Find(const OmMeshDataEntry& entry) {
    if (!table_) {
      return nullptr;
    }

    OmMeshDataEntry* iter =
        std::lower_bound(table_, table_ + numEntries_, entry, compareBySegID);

    if (iter == table_ + numEntries_ || iter->segID != entry.segID) {
      return nullptr;
    }

    return iter;
  }

  void Unmap() {
    file_.reset();
    table_ = nullptr;
    numEntries_ = 0;
  }

  inline bool MapIfNeeded() {
    if (file_) {
      return false;
    }

    map();

    return true;
  }

 private:
  void map() {
    file_.reset(new QFile(fnp_));

    if (!file_->exists()) {
      return;
    }

    if (!file_->open(QIODevice::ReadWrite)) {
      throw om::IoException("could not open");
    }

    uchar* map = file_->map(0, file_->size());
    if (!map) {
      throw om::IoException("could not map");
    }

    file_->close();

    table_ = reinterpret_cast<OmMeshDataEntry*>(map);
    numEntries_ = file_->size() / sizeof(OmMeshDataEntry);
  }

  void setupFile() {
    const ChunkUniqueValues segIDs =
        segmentation_->UniqueValuesDS().Values(coord_, threshold_);

    if (!segIDs.size()) {
      log_infos << "No unique values in " << coord_;
      return;
    }

    file_.reset(new QFile(fnp_));

    if (!file_->open(QIODevice::ReadWrite)) {
      throw om::IoException("could not open");
    }

    file_->resize(segIDs.size() * sizeof(OmMeshDataEntry));

    uchar* map = file_->map(0, file_->size());
    if (!map) {
      throw om::IoException("could not map");
    }

    file_->close();

    table_ = reinterpret_cast<OmMeshDataEntry*>(map);
    numEntries_ = segIDs.size();

    resetTable(segIDs);
    sortTable();
  }

  struct ResetEntry {
    OmMeshDataEntry operator()(const om::common::SegID segID) const {
      return om::meshio_::MakeEmptyEntry(segID);
    }
  };

  void resetTable(const ChunkUniqueValues& segIDs) {
    assert(table_);

    zi::transform(segIDs.begin(), segIDs.end(), table_, ResetEntry());
  }

  void sortTable() {
    assert(table_);

    zi::sort(table_, table_ + numEntries_, compareBySegID);
  }

  static bool compareBySegID(const OmMeshDataEntry& a,
                             const OmMeshDataEntry& b) {
    return a.segID < b.segID;
  }

  QString filePath() {
    const QString volPath =
        segmentation_->Folder()->GetMeshChunkFolderPath(threshold_, coord_);

    if (!QDir(volPath).exists()) {
      segmentation_->Folder()->MakeMeshChunkFolderPath(threshold_, coord_);
    }

    const QString fullPath = QString("%1meshAllocTable.ver2").arg(volPath);

    return fullPath;
  }
};
