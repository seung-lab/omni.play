#pragma once

#include <unordered_map>
#include <vmmlib/vmmlib.h>
using namespace vmml;

#include "common/common.h"
#include "common/colors.h"
#include "yaml-cpp/yaml.h"

namespace YAML {

template <>
struct convert<om::common::Color> {
  static Node encode(const om::common::Color& c) {
    Node node;
    node.push_back(c.red);
    node.push_back(c.green);
    node.push_back(c.blue);
    return node;
  }

  static bool decode(const Node& node, om::common::Color& c) {
    if (!node.IsSequence() || node.size() != 3) {
      return false;
    }

    c.red = node[0].as<uint8_t>();
    c.green = node[1].as<uint8_t>();
    c.blue = node[2].as<uint8_t>();
    return true;
  }
};

template <class T>
struct convert<Vector3<T>> {
  static Node encode(const Vector3<T>& p) {
    Node node;
    node.push_back(p.x);
    node.push_back(p.y);
    node.push_back(p.z);
    return node;
  }

  static bool decode(const Node& node, Vector3<T>& p) {
    if (!node.IsSequence() || node.size() != 3) {
      return false;
    }

    p.x = node[0].as<T>();
    p.y = node[1].as<T>();
    p.z = node[2].as<T>();
    return true;
  }
};

template <class T>
struct convert<Matrix4<T>> {
  static Node encode(const Matrix4<T>& m) {
    Node node;
    for (auto i = 0; i < 16; i++) {
      node.push_back(m.array[i]);
    }
    return node;
  }

  static bool decode(const Node& node, Matrix4<T>& m) {
    if (!node.IsSequence() || node.size() != 16) {
      return false;
    }

    for (auto i = 0; i < 16; i++) {
      m.array[i] = node[i].as<T>();
    }
    return true;
  }
};

template <class T>
struct convert<AxisAlignedBoundingBox<T>> {
  static Node encode(const AxisAlignedBoundingBox<T>& aabb) {
    Node node;
    node["min"] = aabb.getMin();
    node["max"] = aabb.getMax();
    return node;
  }

  static bool decode(const Node& node, AxisAlignedBoundingBox<T>& aabb) {
    if (!node.IsMap() || node.size() != 2) {
      return false;
    }

    aabb.setMin(node["min"].as<T>());
    aabb.setMax(node["max"].as<T>());
    return true;
  }
};

template <class T>
struct convert<std::unordered_set<T>> {
  static Node encode(const std::unordered_set<T>& set) {
    Node node;
    for (auto& n : set) {
      node.push_back(n);
    }
    return node;
  }

  static bool decode(const Node& node, std::unordered_set<T>& set) {
    if (!node.IsSequence()) {
      return false;
    }
    std::unordered_set<T> tmp;

    for (auto& n : node) {
      T val;
      if (!convert<T>::decode(n, val)) {
        return false;
      }
      tmp.insert(val);
    }
    set = std::move(tmp);
    return true;
  }
};

template <class T>
struct convert<std::set<T>> {
  static Node encode(const std::set<T>& set) {
    Node node;
    for (auto& n : set) {
      node.push_back(n);
    }
    return node;
  }

  static bool decode(const Node& node, std::set<T>& set) {
    if (!node.IsSequence()) {
      return false;
    }
    std::set<T> tmp;

    for (auto& n : node) {
      T val;
      if (!convert<T>::decode(n, val)) {
        return false;
      }
      tmp.insert(val);
    }
    set = std::move(tmp);
    return true;
  }
};

template <class TKey, class TValue>
struct convert<std::unordered_map<TKey, TValue>> {
  static Node encode(const std::unordered_map<TKey, TValue>& map) {
    Node node;
    for (auto& n : map) {
      node[n.first] = n.second;
    }
    return node;
  }

  static bool decode(const Node& node, std::unordered_map<TKey, TValue>& map) {
    if (!node.IsMap()) {
      return false;
    }
    std::unordered_map<TKey, TValue> tmp;

    for (auto& n : node) {
      TKey key;
      if (!convert<TKey>::decode(n.first, key)) {
        return false;
      }
      TValue val;
      if (!convert<TValue>::decode(n.second, val)) {
        return false;
      }
      tmp[key] = val;
    }
    map = std::move(tmp);
    return true;
  }
};

}  // namespace YAML
