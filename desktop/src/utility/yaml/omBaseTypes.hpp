#pragma once

#include "common/common.h"
#include "common/colors.h"
#include "yaml-cpp-old/yaml.h"

#include <unordered_set>
#include <unordered_map>
#include <QHash>
#include <zi/for_each.hpp>

class QString;

namespace YAMLold {

inline Emitter& operator<<(Emitter& out, const QString& s) {
  return out << s.toStdString();
}

inline void operator>>(const Node& in, QString& s) {
  std::string str;
  in >> str;

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
  int temp;
  node[0] >> temp;
  c.red = temp;
  node[1] >> temp;
  c.green = temp;
  node[2] >> temp;
  c.blue = temp;
}

template <class T> Emitter& operator<<(Emitter& out, const Vector3<T>& p) {
  out << Flow;
  out << BeginSeq << p.x << p.y << p.z << EndSeq;
  return out;
}

template <class T> void operator>>(const Node& in, Vector3<T>& p) {
  in[0] >> p.x;
  in[1] >> p.y;
  in[2] >> p.z;
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
Emitter& operator<<(Emitter& out, const std::unordered_set<T>& s) {
  out << Flow << BeginSeq;
  FOR_EACH(it, s) { out << *it; }
  out << EndSeq;
  return out;
}

template <class T> void operator>>(const Node& in, std::unordered_set<T>& s) {
  FOR_EACH(it, in) {
    T item;
    *it >> item;
    s.insert(item);
  }
}

 template<class T>
 Emitter &operator<<(Emitter& out, const std::set<T>& s)
 {
     out << Flow << BeginSeq;
     FOR_EACH(it, s)
     {
         out << *it;
     }
     out <<  EndSeq;
     return out;
 }

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
  FOR_EACH(it, in) {
    K key;
    V value;
    it.first() >> key;
    it.second() >> value;
    p.insert(key, value);
  }
}

template <class Key, class T>
YAMLold::Emitter& operator<<(YAMLold::Emitter& out,
                             const std::unordered_map<Key, T>& p) {
  out << YAMLold::BeginMap;
  for (const auto& kv : p) {
    out << YAMLold::Key << kv.first;
    out << YAMLold::Value << kv.second;
  }
  out << YAMLold::EndMap;

  return out;
}

template <class Key, class T>
    void operator>>(const YAMLold::Node& in, std::unordered_map<Key, T>& p) {
  FOR_EACH(it, in) {
    Key key;
    T value;
    it.first() >> key;
    it.second() >> value;
    p[key] = value;
  }
}

}  // namespace YAMLold
