#pragma once

#include "common/common.h"
#include "mesh/io/v2/chunk/omMeshChunkAllocTable.hpp"
#include "mesh/io/v2/chunk/omMeshChunkDataReaderV2.hpp"
#include "mesh/io/v2/chunk/omMeshChunkDataWriterV2.hpp"
#include "mesh/io/v2/omRingBuffer.hpp"
#include "mesh/omMeshCoord.h"
#include "threads/omTaskManager.hpp"
#include "zi/omMutex.h"

class OmMeshChunkAllocTableV2;
class OmMeshChunkDataWriterV2;

class OmMeshFilePtrCache {
 private:
  OmSegmentation* const segmentation_;
  const double threshold_;

  std::map<om::chunkCoord, std::shared_ptr<OmMeshChunkAllocTableV2> > tables_;
  std::map<om::chunkCoord, std::shared_ptr<OmMeshChunkDataWriterV2> > data_;
  zi::rwmutex lock_;

  OmThreadPool threadPool_;

  // limit number of memory-mapped files
  OmRingBuffer<OmMeshChunkAllocTableV2> mappedFiles_;

 public:
  OmMeshFilePtrCache(OmSegmentation* segmentation, const double threshold)
      : segmentation_(segmentation), threshold_(threshold) {
    threadPool_.start();
  }

  ~OmMeshFilePtrCache() { Stop(); }

  void Join() { threadPool_.join(); }

  void Stop() { threadPool_.stop(); }

  void FlushMappedFiles() {
    log_infos << "flushing mesh allocation tables..." << std::flush;
    mappedFiles_.Clear();
    log_infos << "done";
  }

  void AddTaskBack(const std::shared_ptr<zi::runnable> job) {
    threadPool_.push_back(job);
  }

  uint32_t NumTasks() const { return threadPool_.getTaskCount(); }

  void RegisterMappedFile(OmMeshChunkAllocTableV2* table) {
    mappedFiles_.Put(table);
  }

  OmMeshChunkAllocTableV2* GetAllocTable(const om::chunkCoord& coord) {
    zi::rwmutex::write_guard g(lock_);

    if (!tables_.count(coord)) {
      tables_[coord] = std::make_shared<OmMeshChunkAllocTableV2>(
          this, segmentation_, coord, threshold_);
    }

    return tables_[coord].get();
  }

  OmMeshChunkDataWriterV2* GetWriter(const om::chunkCoord& coord) {
    zi::rwmutex::write_guard g(lock_);

    if (!data_.count(coord)) {
      data_[coord] = std::make_shared<OmMeshChunkDataWriterV2>(
          segmentation_, coord, threshold_);
    }

    return data_[coord].get();
  }
};
