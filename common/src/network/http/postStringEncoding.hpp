#pragma once

#include <string>
#include "datalayer/file.h"

namespace om {
namespace network {
namespace http {
template <typename TKey, typename TVal, typename... TRest>
std::string postString(std::pair<TKey, TVal> toPost, TRest... rest) {
  return postPair(url_encode(toPost.first), toPost.second) + "&" +
         postString(rest...);
}
template <typename TKey, typename TVal>
std::string postString(std::pair<TKey, TVal> toPost) {
  return postPair(url_encode(toPost.first), toPost.second);
}
inline std::string postString() { return ""; }
inline std::string postString(const file::path& p) {
  return "@" + boost::filesystem::absolute(p).string();
}

template <typename TVal>
std::string postPair(std::string first, TVal second) {
  return first + "=" + url_encode(second);
}

template <typename TKey, typename TVal>
std::string postPair(const std::string& name, std::map<TKey, TVal> map) {
  std::string result;
  bool first = true;
  for (auto& pair : map) {
    if (!first) {
      result += "&";
    } else {
      first = false;
    }
    result += postPair(std::string(name) + "[" + url_encode(pair.first) + "]",
                       pair.second);
  }
  return result;
}

template <typename TKey, typename TVal>
std::string postPair(const std::string& name,
                     std::unordered_map<TKey, TVal> map) {
  std::string result;
  bool first = true;
  for (auto& pair : map) {
    if (!first) {
      result += "&";
    } else {
      first = false;
    }
    result += postPair(std::string(name) + "[" + url_encode(pair.first) + "]",
                       pair.second);
  }
  return result;
}

template <typename T>
std::string postPair(const std::string& name, std::vector<T> vec) {
  std::string result;
  bool first = true;
  for (auto& elem : vec) {
    if (!first) {
      result += "&";
    } else {
      first = false;
    }
    result += postPair(std::string(name) + "[]", elem);
  }
  return result;
}

inline std::string url_encode(const char* str) {
  char* encoded = curl_easy_escape(nullptr, str, 0);
  std::string result(encoded);
  curl_free(encoded);
  return result;
}

inline std::string url_encode(std::string val) {
  return url_encode(val.c_str());
}

template <typename T>
std::string url_encode(T val) {
  return url_encode(std::to_string(val).c_str());
}
}
}
}
