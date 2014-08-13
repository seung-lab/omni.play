#pragma once
#include "precomp.h"

#include "common/common.h"
#include "common/colors.h"

class QString;

namespace YAML {

template <>
struct convert<QString> {
  static Node encode(const QString& str) { return Node(str.toStdString()); }
  static bool decode(const Node& node, QString& str) {
    std::string s;
    if (!convert<std::string>::decode(node, s)) {
      return false;
    }
    str = QString::fromStdString(s);
    return true;
  }
};

template <class K, class V>
struct convert<QHash<K, V>> {
  static Node encode(const QHash<K, V>& hash) {
    Node n;
    for (auto& key : hash.keys()) {
      n[key] = hash[key];
    }
    return n;
  }
  static bool decode(const Node& node, QHash<K, V>& hash) {
    if (!node.IsMap()) {
      return false;
    }
    QHash<K, V> tmp;

    for (auto& n : node) {
      K key;
      if (!convert<K>::decode(n.first, key)) {
        return false;
      }
      V val;
      if (!convert<V>::decode(n.second, val)) {
        return false;
      }
      tmp[key] = val;
    }
    hash = std::move(tmp);
    return true;
  }
};

}  // namespace YAML
