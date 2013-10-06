#pragma once

#include "common/common.h"
#include "yaml-cpp-old/yaml.h"

#include <QHash>
#include <zi/for_each.hpp>

class QString;

namespace om {
namespace data {
namespace archive {

inline YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const QString& s) {
  return out << s.toStdString();
}

inline void operator>>(const YAMLold::Node& in, QString& s) {
  std::string str = in.as<std::string>();

  if (str == "~")  // NULL Value from YAMLold
    str = "";

  s = QString::fromStdString(str);
}

template <class T>
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const Vector3<T>& p) {
  out << YAMLold::Flow;
  out << YAMLold::BeginSeq << p.x << p.y << p.z << YAMLold::EndSeq;
  return out;
}

template <class T> void operator>>(const YAMLold::Node& in, Vector3<T>& p) {
  in[0] >> p.x;
  in[1] >> p.y;
  in[2] >> p.z;
}

template <class T>
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const Matrix4<T>& p) {
  out << YAMLold::Flow << YAMLold::BeginSeq;
  for (int i = 0; i < 16; i++) {
    out << p.array[i];
  }
  out << YAMLold::EndSeq;
  return out;
}

template <class T> void operator>>(const YAMLold::Node& in, Matrix4<T>& p) {
  T f[16];
  for (int i = 0; i < 16; i++) {
    in[i] >> f[i];
  }
  p.set(f);
}

template <class T>
YAMLold::Emitter& operator<<(YAMLold::Emitter& out,
                          const vmml::AxisAlignedBoundingBox<T>& b) {
  out << YAMLold::BeginMap;
  out << YAMLold::Key << "min" << YAMLold::Value << b.getMin();
  out << YAMLold::Key << "max" << YAMLold::Value << b.getMax();
  out << YAMLold::EndMap;
  return out;
}

template <class T>
    void operator>>(const YAMLold::Node& in, vmml::AxisAlignedBoundingBox<T>& b) {
  Vector3<T> min, max;
  in["min"] >> min;
  in["max"] >> max;
  b = AxisAlignedBoundingBox<T>(min, max);
}

template <class T>
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const std::unordered_set<T>& s) {
  out << YAMLold::Flow << YAMLold::BeginSeq;
  for (const auto& e : s) {
    out << e;
  }
  out << YAMLold::EndSeq;
  return out;
}

template <class T>
    void operator>>(const YAMLold::Node& in, std::unordered_set<T>& s) {
  FOR_EACH(it, in) {
    T item;
    *it >> item;
    s.insert(item);
  }
}

template <class T>
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const std::set<T>& s) {
  out << YAMLold::Flow << YAMLold::BeginSeq;
  FOR_EACH(it, s) { out << *it; }
  out << YAMLold::EndSeq;
  return out;
}

template <class T> void operator>>(const YAMLold::Node& in, std::set<T>& s) {
  FOR_EACH(it, in) {
    T item;
    *it >> item;
    s.insert(item);
  }
}

template <class Key, class T>
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const QHash<Key, T>& p) {
  out << YAMLold::BeginMap;
  FOR_EACH(it, p) {
    out << YAMLold::Key << it.key();
    out << YAMLold::Value << it.value();
  }
  out << YAMLold::EndMap;

  return out;
}

template <class Key, class T>
    void operator>>(const YAMLold::Node& in, QHash<Key, T>& p) {
  for (const auto& kv : p) {
    Key key;
    T value;
    kv.first >> key;
    kv.second >> value;
    p.insert(key, value);
  }
}

};  // namespace archive
};  // namespace data
};  // namespace om
