#pragma once

#include <string>
#include <memory>
#include "network/http/typedefs.hpp"

namespace om {
namespace network {
namespace http {
struct V1 {
};

// Must implement om::network::http::partial_path for each key type.  Tag is
// used to differentiate multiple API versions.
template <typename TKey, typename Tag = V1>
uri path(Tag tag, uri root, const TKey& key) {
  return root << partial_path(tag, key);
}

// Boring default implementation.  Partially specialize for specific data types.
template <typename T, typename VersionTag = V1> struct interface {
  static std::string mime_type() { return "application/octet-stream"; }
  static std::string serialize(std::shared_ptr<T> data) {
    return std::string();
  }
  template <typename TKey>
  static std::shared_ptr<T> deserialize(const TKey& key,
                                        const std::string& data) {
    return std::shared_ptr<T>();
  }
};
}
}
}  // namespace om::network::