#pragma once

#include "common/common.h"
#include "common/colors.h"
#include "yaml-cpp/yaml.h"

#include <QHash>

#include <zi/for_each.hpp>

namespace YAML {

inline Emitter& operator<<(Emitter& out, const QString& s) {
  return out << s.toStdString();
}

inline void operator>>(const Node& in, QString& s) {
  auto str = in.as<std::string>();

  if (str == "~")  // NULL Value from YAML
    str = "";

  s = QString::fromStdString(str);
}

inline Emitter& operator<<(Emitter& out, const om::common::Color& c) {
  out << Flow << BeginSeq;
  out << (uint32_t) c.red;
  out << (uint32_t) c.green;
  out << (uint32_t) c.blue;
  out << EndSeq;

  return out;
}

inline void operator>>(const Node& node, om::common::Color& c) {
  // workaround: Reading into uint8_ts appears to be bugged out.
  c.red = node[0].as<uint8_t>();
  c.green = node[1].as<uint8_t>();
  c.blue = node[2].as<uint8_t>();
}

template <class T> Emitter& operator<<(Emitter& out, const Vector3<T>& p) {
  out << Flow;
  out << BeginSeq << p.x << p.y << p.z << EndSeq;
  return out;
}

template <class T> void operator>>(const Node& in, Vector3<T>& p) {
  p.x = in[0].as<T>();
  p.y = in[1].as<T>();
  p.z = in[2].as<T>();
}

template <class T> Emitter& operator<<(Emitter& out, const Matrix4<T>& p) {
  out << Flow << BeginSeq;
  for (int i = 0; i < 16; i++) {
    out << p.array[i];
  }
  out << EndSeq;
  return out;
}

template <class T> void operator>>(const Node& in, Matrix4<T>& p) {
  T f[16];
  for (int i = 0; i < 16; i++) {
    in[i] >> f[i];
  }
  p.set(f);
}

template <class T>
Emitter& operator<<(Emitter& out, const vmml::AxisAlignedBoundingBox<T>& b) {
  out << BeginMap;
  out << Key << "min" << Value << b.getMin();
  out << Key << "max" << Value << b.getMax();
  out << EndMap;
  return out;
}

template <class T>
    void operator>>(const Node& in, vmml::AxisAlignedBoundingBox<T>& b) {
  Vector3<T> min, max;
  in["min"] >> min;
  in["max"] >> max;
  b = AxisAlignedBoundingBox<T>(min, max);
}

template <class T>
Emitter& operator<<(Emitter& out, const std::unordered_set<T>& set) {
  out << Flow << BeginSeq;
  for (const auto& s : set) {
    out << s;
  }
  out << EndSeq;
  return out;
}

template <class T> void operator>>(const Node& in, std::unordered_set<T>& s) {
  FOR_EACH(it, in) {
    T item = it->as<T>();
    s.insert(item);
  }
}

// template<class T>
// Emitter &operator<<(Emitter& out, const std::set<T>& s)
// {
//     out << Flow << BeginSeq;
//     FOR_EACH(it, s)
//     {
//         out << *it;
//     }
//     out <<  EndSeq;
//     return out;
// }

template <class T> void operator>>(const Node& in, std::set<T>& s) {
  FOR_EACH(it, in) {
    T item;
    *it >> item;
    s.insert(item);
  }
}

template <class K, class V>
Emitter& operator<<(Emitter& out, const QHash<K, V>& p) {
  out << BeginMap;
  FOR_EACH(it, p) {
    out << Key << it.key();
    out << Value << it.value();
  }
  out << EndMap;

  return out;
}

template <class K, class V> void operator>>(const Node& in, QHash<K, V>& p) {
  for (const auto& kv : in) {
    K key;
    V value;
    kv.first >> key;
    kv.second >> value;
    p.insert(key, value);
  }
}

}  // namespace YAML
