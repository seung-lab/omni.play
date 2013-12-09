#pragma once

#include "common/exception.h"
#include "utility/malloc.hpp"

#include <vector>
#include <QFile>
#include <QTextStream>

namespace om {
namespace file {
namespace old {

void openFileRO(QFile& file);
void openFileRW(QFile& file);
void openFileWO(QFile& file);
void openFileAppend(QFile& file);

int64_t numBytes(const std::string& fnp);

void resizeFileNumBytes(QFile* file, const int64_t newSize);
inline void resizeFileNumBytes(QFile& file, const int64_t newSize) {
  resizeFileNumBytes(&file, newSize);
}

template <typename T>
void resizeFileNumElements(QFile& file, const int64_t numElements) {
  resizeFileNumBytes(&file, sizeof(T) * numElements);
}

template <typename T>
void resizeFileNumElements(QFile* file, const int64_t numElements) {
  resizeFileNumBytes(file, sizeof(T) * numElements);
}

void rmFile(const std::string& fnp);
void mvFile(const std::string& old_fnp, const std::string& new_fnp);
void cpFile(const std::string& old_fnp, const std::string& new_fnp);

bool exists(const std::string& fnp);

std::string tempPath();

template <class PTR> void openFileRO(PTR& file, const std::string& fnp) {
  file.reset(new QFile(QString::fromStdString(fnp)));
  if (!file->open(QIODevice::ReadOnly)) {
    throw om::IoException("could not open file read only", fnp);
  }
}

template <class PTR> void openFileAppend(PTR& file, const std::string& fnp) {
  file.reset(new QFile(QString::fromStdString(fnp)));
  if (!file->open(QIODevice::Append)) {
    throw om::IoException("could not open file read/write append", fnp);
  }
}

template <class PTR> void openFileRW(PTR& file, const std::string& fnp) {
  file.reset(new QFile(QString::fromStdString(fnp)));
  if (!file->open(QIODevice::ReadWrite)) {
    throw om::IoException("could not open file read/write");
  }
}

template <class PTR> void openFileWO(PTR& file, const std::string& fnp) {
  file.reset(new QFile(QString::fromStdString(fnp)));

  if (!file->open(QIODevice::WriteOnly)) {
    throw om::IoException("could not open file for writing");
  }
}

template <typename T> T* mapFile(QFile* file) {
  uchar* map = file->map(0, file->size());

  if (!map) {
    throw om::IoException("could not map file");
  }

  file->close();

  return reinterpret_cast<T*>(map);
}

template <typename T, class PTR> T* mapFile(PTR& file) {
  uchar* map = file->map(0, file->size());

  if (!map) {
    throw om::IoException("could not map file");
  }

  file->close();

  return reinterpret_cast<T*>(map);
}

template <typename T> std::shared_ptr<T> readAll(QFile* file) {
  const int64_t numBytes = file->size();

  if (0 != numBytes % sizeof(T)) {
    throw om::IoException("file size not even multiple of sizeof(type)");
  }

  std::shared_ptr<T> ret =
      om::mem::Malloc<T>::NumBytes(numBytes, om::mem::ZeroFill::DONT);

  char* dataChar = reinterpret_cast<char*>(ret.get());

  const int64_t numBytesRead = file->read(dataChar, numBytes);

  if (numBytesRead != numBytes) {
    throw om::IoException("could not read entire file");
  }

  return ret;
}

template <typename T> std::shared_ptr<T> readAll(QFile& file) {
  return readAll<T>(&file);
}

template <typename T> void writeVec(QFile& file, const std::vector<T>& vec) {
  resizeFileNumElements<T>(file, vec.size());

  const char* data = reinterpret_cast<const char*>(&vec[0]);

  const int64_t numBytes = vec.size() * sizeof(T);

  const int numBytesWritten = file.write(data, numBytes);

  if (numBytesWritten != numBytes) {
    throw om::IoException("could not fully write file");
  }
}

template <typename T>
void writeNumElements(QFile& file, const std::shared_ptr<T> ptr,
                      const int64_t numElements) {
  const int64_t numBytes = numElements * sizeof(T);

  resizeFileNumBytes(file, numBytes);

  const char* data = reinterpret_cast<const char*>(ptr.get());

  const int numBytesWritten = file.write(data, numBytes);

  if (numBytesWritten != numBytes) {
    throw om::IoException("could not fully write file");
  }
}

template <typename T>
void createFileNumElements(const std::string& fnp, const int64_t numElements) {
  QFile file(QString::fromStdString(fnp));

  openFileWO(file);

  std::shared_ptr<T> empty =
      om::mem::Malloc<T>::NumElements(numElements, om::mem::ZeroFill::ZERO);

  writeNumElements(file, empty, numElements);
}

template <typename T>
void createFileFromData(const std::string& fnp, const std::shared_ptr<T> ptr,
                        const int64_t numElements) {
  QFile file(QString::fromStdString(fnp));

  openFileWO(file);

  writeNumElements(file, ptr, numElements);
}

template <typename T> void writeStrings(QFile& file, const T& strings) {
  QTextStream out(&file);

  FOR_EACH(iter, strings) { out << QString::fromStdString(*iter) << "\n"; }
}

template <typename T, typename PROGRESS>
void writeStrings(QFile& file, const T& strings, PROGRESS* progress) {
  progress->SetTotal(strings.size());

  QTextStream out(&file);

  FOR_EACH(iter, strings) {
    out << QString::fromStdString(*iter) << "\n";
    progress->SetDone(1);
  }
}

}  // namespace file
}
}  // namespace om
