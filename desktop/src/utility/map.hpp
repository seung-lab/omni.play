#pragma once

#include "precomp.h"

namespace om {
namespace map {

template<typename K, typename V>
std::map<K,V> ToStdMap(std::unordered_map<K,V> unorderedMap) {
  std::map<K,V> stdMap;
  for (auto element : unorderedMap) {
    stdMap.insert(std::pair<K,V>(element.first, element.second));
  }
  return stdMap;
}

template<typename K, typename V>
std::unordered_map<K,V> ToUnorderedMap(std::map<K,V> map) {
  std::unordered_map<K,V> unorderedMap;
  for (auto element : map) {
    unorderedMap.insert(std::pair<K,V>(element.first, element.second));
  }
  return unorderedMap;
}

} // namespace map
} // namespace om
