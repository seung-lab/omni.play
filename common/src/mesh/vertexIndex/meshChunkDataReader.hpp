#pragma once

#include "common/common.h"
#include "mesh/vertexIndex/chunkTypes.hpp"

#include <fstream>

namespace om {
namespace mesh {

class chunkDataReader {
 private:
  const file::path fnp_;

 public:
  chunkDataReader(const file::path& fnp) : fnp_(fnp) {}

  ~chunkDataReader() {}

  // no locking needed
  template <typename T>
  std::shared_ptr<T> Read(const FilePart& entry) {
    const int64_t numBytes = entry.totalBytes;

    assert(numBytes);

    auto ret = mem::Malloc<T>::NumBytes(numBytes, mem::ZeroFill::DONT);

    char* dataCharPtr = reinterpret_cast<char*>(ret.get());

    std::ifstream reader(fnp_.c_str(), std::ios::in | std::ios::binary);

    if (!reader.seekg(entry.offsetIntoFile)) {
      std::stringstream ss;
      ss << "could not seek to " << entry.offsetIntoFile;
      throw IoException(ss.str());
    }

    reader.read(dataCharPtr, numBytes);

    if (reader.fail()) {
      log_errors(io) << "could not read data; numBytes is " << numBytes;
      throw IoException("could not read fully file", fnp_.string());
    }

    return ret;
  }
};

}  // namespace mesh
}  // namespace om
