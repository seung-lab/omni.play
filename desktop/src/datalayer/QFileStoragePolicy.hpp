#pragma once

#include "datalayer/file.h"
#include <QFile>
#include <QDataStream>

namespace om {
namespace data {

template <typename T>
class QFileStoragePolicy {
 public:
  QFileStoragePolicy(file::path fnp) : fnp_(fnp) { load(); }

  size_t size() const { return data_.size(); }
  void resize(size_t n, const T& val) { data_.resize(n, val); }
  void resize(size_t n) { data_.resize(n); }
  void reserve(size_t n) { data_.reserve(n); }
  T& doGet(size_t i) { return data_[i]; }
  const T& doGet(size_t i) const { return data_[i]; }
  void flush() { save(); }

 private:
  static const int CurrentFileVersion = 1;

  void load() {
    const QString path = QString::fromStdString(fnp_.string());

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
      throw IoException("error reading file " + fnp_.string());
    }

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_4_6);

    int version;
    in >> version;

    if (CurrentFileVersion != version) {
      throw IoException("versions differ: " + fnp_.string());
    }

    int size;
    in >> size;
    for (int i = 0; i < size; ++i) {
      T d;
      in >> d;
      data_.push_back(d);
    }

    if (!in.atEnd()) {
      throw IoException("corrupt file? " + fnp_.string());
    }
  }

  void save() {
    const QString path = QString::fromStdString(fnp_.string());

    QFile file(path);

    if (!file.open(QIODevice::WriteOnly)) {
      throw IoException("could not write file " + fnp_.string());
    }

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_4_6);

    out << CurrentFileVersion;

    int size = data_.size();
    out << size;
    for (auto& d : data_) {
      out << d;
    }
  }

  file::path fnp_;
  std::vector<T> data_;
};
}
}  // namespace om::datalayer::
