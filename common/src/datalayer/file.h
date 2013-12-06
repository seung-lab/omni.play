#pragma once

#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>

#include "common/exception.h"
#include "zi/concurrency/rwmutex.hpp"

namespace boost {
namespace filesystem {
const directory_iterator& begin(const directory_iterator& iter);
directory_iterator end(const directory_iterator&);
}
}

namespace om {
namespace file {

using boost::filesystem::path;

uint64_t numBytes(const path& fnp);

void resizeFileNumBytes(const path& fnp, const int64_t newSize);

template <typename T>
void resizeFileNumElements(const path& fnp, const int64_t numElements) {
  resizeFileNumBytes(fnp, sizeof(T) * numElements);
}

using boost::filesystem::exists;
using boost::filesystem::absolute;
using boost::filesystem::extension;

bool IsFolderEmpty(const path& dirName);
bool IsFolder(const path& dirName);

bool MkDir(const path& dirName);
void RemoveDir(const path& dirName);  // rm -rf

void CreateFolder(const path& fullPath, zi::rwmutex& lock);

void MoveFile(const path& from, const path& to);
void RmFile(const path& fileName);
void MoveAllFiles(const path& fromDir, const path& toDir);
void CopyFile(const path& from, const path& to, bool overwrite = false);

void Symlink(const path& fromDir, const path& toDir);
bool IsSymlink(const path& fileName);

path tempPath();
path tempFile();

template <typename T>
void readAll(const path& fnp, std::vector<T>& vec) {
  const size_t bytes = numBytes(fnp);
  const size_t elements = bytes / sizeof(T);
  const size_t extra = bytes % sizeof(T);

  if (0 != extra) {
    throw IoException("file size not even multiple of sizeof(type)");
  }

  vec.clear();
  vec.resize(elements);
  char* dataChar = reinterpret_cast<char*>(vec.data());

  std::fstream file(fnp.c_str(), std::fstream::in | std::fstream::binary);
  file.read(dataChar, bytes);
  const int64_t bytesRead = file.gcount();

  if (bytesRead != bytes) {
    throw IoException("could not read entire file");
  }
}

template <typename T>
void writeAll(const path& fnp, const std::vector<T>& vec) {
  resizeFileNumElements<T>(fnp, vec.size());

  const char* data = reinterpret_cast<const char*>(&vec[0]);

  const int64_t bytes = vec.size() * sizeof(T);

  std::fstream file(fnp.c_str(), std::fstream::out | std::fstream::binary);

  if (!file.good()) {
    throw IoException("could not fully write file", fnp.string());
  }

  file.write(data, bytes);
}

template <typename T>
void createFileNumElements(const path& fnp, const int64_t numElements) {
  const int64_t numBytes = numElements * sizeof(T);

  resizeFileNumBytes(fnp, numBytes);
}

template <typename T>
void writeNumElements(const path& fnp, const std::shared_ptr<T> ptr,
                      const int64_t numElements) {
  const int64_t numBytes = numElements * sizeof(T);

  resizeFileNumBytes(fnp, numBytes);

  const char* data = reinterpret_cast<const char*>(ptr.get());

  std::fstream file(fnp.c_str(), std::fstream::out | std::fstream::binary);

  if (!file.good()) {
    throw IoException("could not fully write file", fnp.string());
  }

  file.write(data, numBytes);
}

template <typename T>
void writeStrings(const path& file, const T& strings) {
  std::ofstream out(file.c_str());
  log_debugs << "Writing: " << file.string();
  for (auto& s : strings) {
    out << s << "\n";
  }
  out.close();
}

template <typename T, typename PROGRESS>
void writeStrings(const path& file, const T& strings, PROGRESS* progress) {
  progress->SetTotal(strings.size());

  std::ofstream out(file.c_str());
  log_debugs << "Writing: " << file.string();

  for (auto& s : strings) {
    out << s << "\n";
    progress->SetDone(1);
  }
  out.close();
}

}  // namespace file
}  // namespace om
