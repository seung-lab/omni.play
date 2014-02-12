#pragma once

#include "common/common.h"
#include <QFile>

namespace om {
namespace file {

template <class PTR> void openFileRO(PTR& file, const std::string& fnp) {
  file = std::make_unique<QFile>(QString::fromStdString(fnp));
  if (!file->open(QIODevice::ReadOnly)) {
    throw om::IoException("could not open file read only", fnp);
  }
}

template <class PTR> void openFileAppend(PTR& file, const std::string& fnp) {
  file = std::make_unique<QFile>(QString::fromStdString(fnp));
  if (!file->open(QIODevice::Append)) {
    throw om::IoException("could not open file read/write append", fnp);
  }
}

template <class PTR> void openFileRW(PTR& file, const std::string& fnp) {
  file = std::make_unique<QFile>(QString::fromStdString(fnp));
  if (!file->open(QIODevice::ReadWrite)) {
    throw om::IoException("could not open file read/write", fnp);
  }
}

template <class PTR> void openFileWO(PTR& file, const std::string& fnp) {
  file = std::make_unique<QFile>(QString::fromStdString(fnp));

  if (!file->open(QIODevice::WriteOnly)) {
    throw om::IoException("could not open file for writing", fnp);
  }
}

inline void openFileRO(QFile& file) {
  if (!file.open(QIODevice::ReadOnly)) {
    throw om::IoException("could not open file read only",
                          file.fileName().toStdString());
  }
}

inline void openFileRW(QFile& file) {
  if (!file.open(QIODevice::ReadWrite)) {
    throw om::IoException("could not open file read/write",
                          file.fileName().toStdString());
  }
}

inline void openFileWO(QFile& file) {
  if (!file.open(QIODevice::WriteOnly | QFile::Truncate)) {
    throw om::IoException("could not open file for write",
                          file.fileName().toStdString());
  }
}

inline void openFileAppend(QFile& file) {
  if (!file.open(QIODevice::Append)) {
    throw om::IoException("could not open file for write",
                          file.fileName().toStdString());
  }
}
}
}  // namespace om::file::
