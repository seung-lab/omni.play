#pragma once

#include "common/common.h"

namespace om {
namespace mesh {

struct FilePart {
  uint64_t offsetIntoFile;
  uint64_t numElements;
  uint64_t totalBytes;
  uint64_t count;
};

struct DataEntry {
  common::SegID segID;
  bool wasMeshed;
  bool hasMeshData;
  FilePart vertexIndex;
  FilePart vertexData;
  FilePart stripData;
  FilePart trianData;
};

static DataEntry MakeEmptyEntry(const common::SegID segID) {
  static const FilePart empty = { 0, 0, 0, 0 };

  DataEntry entry;
  entry.segID = segID;
  entry.wasMeshed = false;
  entry.hasMeshData = false;
  entry.vertexIndex = empty;
  entry.vertexData = empty;
  entry.stripData = empty;
  entry.trianData = empty;

  return entry;
}

}  // namespace mesh
}  // namespace om
