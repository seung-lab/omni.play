#include "datalayer/fs/omFile.hpp"

namespace om {
namespace file {
namespace old {

int64_t numBytes(const std::string& fnp) {
  if (!exists(fnp)) {
    throw om::IoException("file not found", fnp);
  }
  return QFile(QString::fromStdString(fnp)).size();
}

void openFileRO(QFile& file) {
  if (!file.open(QIODevice::ReadOnly)) {
    throw om::IoException("could not open file read only");
  }
}

void openFileRW(QFile& file) {
  if (!file.open(QIODevice::ReadWrite)) {
    throw om::IoException("could not open file read/write");
  }
}

void openFileWO(QFile& file) {
  if (!file.open(QIODevice::WriteOnly | QFile::Truncate)) {
    throw om::IoException("could not open file for write");
  }
}

void openFileAppend(QFile& file) {
  if (!file.open(QIODevice::Append)) {
    throw om::IoException("could not open file for write");
  }
}

void resizeFileNumBytes(QFile* file, const int64_t numBytes) {
  if (!file->resize(numBytes)) {
    throw om::IoException("could not resize file to " +
                          om::string::num(numBytes) + " bytes");
  }
}

void rmFile(const std::string& fnp) {
  const QString f = QString::fromStdString(fnp);

  if (QFile::exists(f)) {
    if (!QFile::remove(f)) {
      throw om::IoException("could not remove previous file");
    }
  }
}

bool exists(const std::string& fnp) {
  return QFile::exists(QString::fromStdString(fnp));
}

std::string tempPath() { return QDir::tempPath().toStdString(); }

void mvFile(const std::string& old_fnp, const std::string& new_fnp) {
  try {
    boost::filesystem::rename(old_fnp, new_fnp);
  }
  catch (...) {
    throw om::IoException("could not mv file");
  }
}

void cpFile(const std::string& from_fnp, const std::string& to_fnp) {
  try {
    rmFile(to_fnp);
    boost::filesystem::copy_file(from_fnp, to_fnp);
  }
  catch (...) {
    throw om::IoException("could not mv file");
  }
}
}
}
}  // namespace
