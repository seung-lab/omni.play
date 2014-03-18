#pragma once

#include "precomp.h"

namespace om {
namespace file {

void uncompressFileToFile(const std::string& in_fnp,
                          const std::string& out_fnp);

void compressToFileNumBytes(char const* const data, const int64_t numBytes,
                            const std::string& out_fnp);

template <class T>
void compressToFileNumElements(T const* const data, const int64_t numElements,
                               const std::string& out_fnp) {
  compressToFileNumBytes(reinterpret_cast<char const* const>(data),
                         sizeof(T) * numElements, out_fnp);
}

template <class T>
void compressToFileNumElements(const std::shared_ptr<T>& data,
                               const int64_t numElements,
                               const std::string& out_fnp) {
  compressToFileNumBytes(reinterpret_cast<char const* const>(data.get()),
                         sizeof(T) * numElements, out_fnp);
}

template <class T>
void compressToFileVector(const std::vector<T>& data,
                          const std::string& out_fnp) {
  compressToFileNumElements(&data.front(), data.size(), out_fnp);
}

}  // namespace file
}  // namespace om
