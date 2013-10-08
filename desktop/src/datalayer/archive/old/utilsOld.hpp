#pragma once

#include "common/common.h"
#include <QDataStream>

template <class T>
inline QDataStream& operator<<(QDataStream& out, const Vector3<T>& v) {
  out << v.array[0];
  out << v.array[1];
  out << v.array[2];
  return out;
}

template <class T>
    inline QDataStream& operator>>(QDataStream& in, Vector3<T>& v) {
  in >> v.array[0];
  in >> v.array[1];
  in >> v.array[2];
  return in;
}

template <class T>
inline QDataStream& operator<<(QDataStream& out,
                               const AxisAlignedBoundingBox<T>& d) {
  vmml::Vector3<int> min = d.getMin();
  vmml::Vector3<int> max = d.getMax();
  bool dirty = d.isDirty();
  bool empty = d.isEmpty();

  out << min;
  out << max;
  out << dirty;
  out << empty;

  return out;
}

template <class T>
    inline QDataStream& operator>>(QDataStream& in,
                                   AxisAlignedBoundingBox<T>& d) {
  vmml::Vector3<int> min;
  vmml::Vector3<int> max;
  bool dirty;
  bool empty;

  in >> min;
  in >> max;
  in >> dirty;
  in >> empty;

  vmml::AxisAlignedBoundingBox<int> temp(min, max);
  d.setDirty(dirty);
  d.setEmpty(empty);
  d = temp;

  return in;
}

template <class T>
inline QDataStream& operator<<(QDataStream& out, const Matrix4<T>& m) {
  for (int i = 0; i < 16; ++i) {
    out << m.array[i];
  }
  return out;
}

template <class T>
    inline QDataStream& operator>>(QDataStream& in, Matrix4<T>& m) {
  for (int i = 0; i < 16; ++i) {
    in >> m.array[i];
  }
  return in;
}
