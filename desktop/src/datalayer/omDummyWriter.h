#pragma once

#include "common/common.h"
#include "datalayer/omIDataWriter.h"

class OmDummyWriter : public om::common::IDataWriter {
 public:
  OmDummyWriter(const std::string &) {}

  void open() {};
  void close() {};
  void flush() {
    log_infos << __FUNCTION__
              << ": write operation should not have happened...";
  }

  // file
  void create() {
    log_infos << __FUNCTION__
              << ": write operation should not have happened...";
  }

  // group
  void group_delete(const OmDataPath &) {
    log_infos << __FUNCTION__
              << ": write operation should not have happened...";
  }

  // image I/O
  void allocateChunkedDataset(const OmDataPath &, const Vector3i &,
                              const Vector3i &, const OmVolDataType) {
    log_infos << __FUNCTION__
              << ": write operation should not have happened...";
  }

  void writeChunk(const OmDataPath &, DataBbox, OmDataWrapperPtr) {
    log_infos << __FUNCTION__
              << ": write operation should not have happened...";
  }

  // data set raw
  void writeDataset(const OmDataPath &, int, const OmDataWrapperPtr) {
    log_infos << __FUNCTION__
              << ": write operation should not have happened...";
  }
};
