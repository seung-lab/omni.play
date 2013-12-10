#pragma once

#include "datalayer/fs/omFileNames.hpp"
#include "mesh/io/v2/chunk/omMemMappedAllocFile.hpp"
#include "utility/lockedObjects.hpp"

class OmMeshFilePtrCache;

class OmMeshChunkAllocTableV2 {
 private:
  OmMeshFilePtrCache* const filePtrCache_;
  std::unique_ptr<OmMemMappedAllocFile> file_;

  om::utility::LockedSet<om::common::SegID> segsBeingSaved_;
  zi::rwmutex lock_;

 public:
  OmMeshChunkAllocTableV2(OmMeshFilePtrCache* filePtrCache, OmSegmentation* seg,
                          const om::coords::Chunk& coord,
                          const double threshold)
      : filePtrCache_(filePtrCache),
        file_(new OmMemMappedAllocFile(seg, coord, threshold)) {}

  bool CreateIfNeeded() {
    zi::rwmutex::write_guard g(lock_);
    if (file_->CreateIfNeeded()) {
      registerMappedFile();
      return true;
    }
    return false;
  }

  // avoid over-lapping writes for meshes with same segment ID
  bool CanContinueMeshSave(const om::common::SegID segID) {
    if (segsBeingSaved_.insertSinceWasntPresent(segID)) {
      return true;
    }
    log_infos << "mesh already being saved...";
    return false;
  }

  void SegmentMeshSaveDone(const om::common::SegID segID) {
    segsBeingSaved_.erase(segID);
  }

  void Unmap() {
    zi::rwmutex::write_guard g(lock_);
    file_->Unmap();
  }

  bool Contains(const OmMeshDataEntry& newEntry) {
    zi::rwmutex::write_guard g(lock_);

    map();

    OmMeshDataEntry* entry = file_->Find(newEntry);
    if (!entry) {
      return false;
    }
    return true;
  }

  inline bool Contains(const om::common::SegID segID) {
    OmMeshDataEntry e;
    e.segID = segID;
    return Contains(e);
  }

  void Set(const OmMeshDataEntry& newEntry) {
    zi::rwmutex::write_guard g(lock_);

    map();

    OmMeshDataEntry* entry = file_->Find(newEntry);
    if (!entry) {
      throw om::IoException("unknown segment ID");
    }

    assert(newEntry.segID == entry->segID);

    (*entry) = newEntry;
  }

  const OmMeshDataEntry Find(const om::common::SegID segID) {
    zi::rwmutex::write_guard g(lock_);

    map();

    OmMeshDataEntry e;
    e.segID = segID;

    OmMeshDataEntry* entry = file_->Find(e);
    if (!entry) {
      throw om::IoException("unknown segment ID");
    }

    return *entry;
  }

  bool CheckEverythingWasMeshed() {
    zi::rwmutex::write_guard g(lock_);

    map();

    return file_->CheckEverythingWasMeshed();
  }

 private:
  inline void map() {
    if (file_->MapIfNeeded()) {
      registerMappedFile();
    }
  }

  void registerMappedFile();
};
