#pragma once

namespace om {
namespace mesh {

struct filePart {
  uint64_t offsetIntoFile;
  uint64_t numElements;
  uint64_t totalBytes;
  uint64_t count;
};

struct dataEntry {
  common::segId segID;
  bool wasMeshed;
  bool hasMeshData;
  filePart vertexIndex;
  filePart vertexData;
  filePart stripData;
  filePart trianData;
};

static dataEntry MakeEmptyEntry(const common::segId segID) {
  static const filePart empty = { 0, 0, 0, 0 };

  dataEntry entry;
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
