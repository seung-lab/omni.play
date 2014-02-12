#pragma once

#include <QDataStream>

template <typename T>
QDataStream& operator<<(QDataStream& out, const std::vector<T>& vec) {
  out << (quint32) vec.size();

  // TODO: make faster
  for (auto& e : vec) {
    out << e;
  }

  return out;
}

template <typename T>
    QDataStream& operator>>(QDataStream& in, std::vector<T>& vec) {
  quint32 size;
  in >> size;

  vec.resize(size);

  for (auto i = 0; i < size; ++i) {
    in >> vec[i];
  }

  return in;
}
