#pragma once

#include "common/logging.h"
#include "utility/resourcePool.hpp"
#include "datalayer/paths.hpp"
#include "zi/utility.h"

#include <memory>
#include <map>
#include <unordered_map>
#include <vector>
#include <curl/curl.h>
#include "yaml-cpp/yaml.h"

namespace om {
namespace network {
#define SET_OPT(h, opt, val, bad_ret)                                  \
  err = curl_easy_setopt(h, opt, val);                                 \
  if (err != CURLE_OK) {                                               \
    log_debugs << "Failed to set " << #opt << curl_easy_strerror(err); \
    return bad_ret;                                                    \
  }

class IHTTPRefreshable {
 public:
  virtual void Refresh(const std::string& result) = 0;
};

class HTTP : private SingletonBase<HTTP> {
 public:
  static std::string GET(const std::string& uri) {
    log_debugs << "HTTP GET: " << uri;
    typename handle_pool::Lease h(instance().HandlePool, true);
    if (!h) {
      log_debugs << "Unable to get curl handle!";
      return std::string();
    }

    CURLcode err;
    SET_OPT(h->Handle, CURLOPT_URL, uri.c_str(), std::string());
    SET_OPT(h->Handle, CURLOPT_FOLLOWLOCATION, 1, std::string());
    SET_OPT(h->Handle, CURLOPT_WRITEFUNCTION, &write_data, std::string());
    SET_OPT(h->Handle, CURLOPT_USERAGENT, "Omni", std::string());

    std::stringstream ss;
    SET_OPT(h->Handle, CURLOPT_WRITEDATA, &ss, std::string());
    SET_OPT(h->Handle, CURLOPT_COOKIEJAR, file::Paths::CookieFile().c_str(),
            std::string());
    SET_OPT(h->Handle, CURLOPT_SSL_VERIFYPEER, 0, std::string());

    err = curl_easy_perform(h->Handle);
    if (err) {
      log_debugs << "CURL Error getting: " << uri << " "
                 << curl_easy_strerror(err);
      return std::string();
    }

    auto resp = getResponseCode(h->Handle);
    if (resp != 200) {
      return std::string();
    }

    auto str = ss.str();
    log_debugs << "HTTP Response: " << str;
    return str;
  }

  template <typename T>
  static typename std::enable_if<std::is_base_of<IHTTPRefreshable, T>::value,
                                 std::shared_ptr<T>>::type
  GET(const std::string& uri) {
    std::shared_ptr<T> ret(new T());
    ret->Refresh(GET(uri));
    return ret;
  }

  template <typename T>
  static std::shared_ptr<T> GET_JSON(const std::string& uri) {
    std::string response = GET(uri);
    if (response.empty()) {
      return std::shared_ptr<T>();
    }

    std::shared_ptr<T> val;
    try {
      auto node = YAML::Load(response);
      val.reset(new T(node.as<T>()));
    }
    catch (YAML::Exception e) {
      log_debugs << "Failed loading JSON: " << e.what();
    }
    return val;
  }

  static bool PUT(const std::string& uri, const std::string& data) {
    log_debugs << "HTTP PUT: " << uri;
    typename handle_pool::Lease h(instance().HandlePool, true);
    if (!h) {
      log_debugs << "Unable to get curl handle!";
      return false;
    }

    CURLcode err;
    SET_OPT(h->Handle, CURLOPT_URL, uri.c_str(), false);
    SET_OPT(h->Handle, CURLOPT_FOLLOWLOCATION, 1, false);
    SET_OPT(h->Handle, CURLOPT_USERAGENT, "Omni", false);

    SET_OPT(h->Handle, CURLOPT_READFUNCTION, &read_data, false);
    std::stringstream ss(data);
    SET_OPT(h->Handle, CURLOPT_READDATA, &ss, false);
    SET_OPT(h->Handle, CURLOPT_COOKIEJAR, file::Paths::CookieFile().c_str(),
            false);
    SET_OPT(h->Handle, CURLOPT_SSL_VERIFYPEER, 0, false);
    SET_OPT(h->Handle, CURLOPT_UPLOAD, 1, false);

    err = curl_easy_perform(h->Handle);
    if (err) {
      log_debugs << "CURL Error uploading: " << uri << curl_easy_strerror(err);
      return false;
    }

    auto resp = getResponseCode(h->Handle);
    if (resp != 200) {
      return false;
    }

    return true;
  }

  template <typename T>
  static bool PUT_JSON(const std::string& uri, const T& val) {
    try {
      YAML::Emitter e;
      e.SetSeqFormat(YAML::Flow);
      e.SetMapFormat(YAML::Flow);
      e << val;
      PUT(uri, std::string(e.c_str(), e.size()));
      return true;
    }
    catch (YAML::Exception e) {
      log_debugs << "Failed converting to JSON: " << e.what();
      return false;
    }
  }

  template <typename... TRest>
  static boost::optional<std::string> POST(const std::string& uri,
                                           TRest... rest) {
    log_debugs << "HTTP POST: " << uri;
    typename handle_pool::Lease h(instance().HandlePool, true);
    if (!h) {
      log_debugs << "Unable to get curl handle!";
      return false;
    }

    CURLcode err;
    SET_OPT(h->Handle, CURLOPT_URL, uri.c_str(), false);
    SET_OPT(h->Handle, CURLOPT_FOLLOWLOCATION, 1, false);
    SET_OPT(h->Handle, CURLOPT_USERAGENT, "Omni", false);

    SET_OPT(h->Handle, CURLOPT_POST, 1, false);

    auto post = postString(rest...);
    log_debugs << "Posting: " << post;
    SET_OPT(h->Handle, CURLOPT_POSTFIELDS, post.c_str(), false);
    if (post.empty()) {
      SET_OPT(h->Handle, CURLOPT_POSTFIELDSIZE, 0, false);
    }

    SET_OPT(h->Handle, CURLOPT_WRITEFUNCTION, &write_data, false);
    std::stringstream ss;
    SET_OPT(h->Handle, CURLOPT_WRITEDATA, &ss, false);

    SET_OPT(h->Handle, CURLOPT_COOKIEJAR, file::Paths::CookieFile().c_str(),
            false);
    SET_OPT(h->Handle, CURLOPT_SSL_VERIFYPEER, 0, false);

    err = curl_easy_perform(h->Handle);
    if (err) {
      log_debugs << "CURL Error getting: " << curl_easy_strerror(err);
      return false;
    }

    auto resp = getResponseCode(h->Handle);
    if (resp != 200) {
      return std::string();
    }

    auto str = ss.str();
    log_debugs << "HTTP Response: " << str;
    return str;
  }

 private:
  template <typename TKey, typename TVal, typename... TRest>
  static std::string postString(std::pair<TKey, TVal> toPost, TRest... rest) {
    return postPair(url_encode(toPost.first), toPost.second) + "&" +
           postString(rest...);
  }
  template <typename TKey, typename TVal>
  static std::string postString(std::pair<TKey, TVal> toPost) {
    return postPair(url_encode(toPost.first), toPost.second);
  }
  static std::string postString() { return ""; }
  static std::string postString(const file::path& p) {
    return "@" + boost::filesystem::absolute(p).string();
  }

  template <typename TVal>
  static std::string postPair(std::string first, TVal second) {
    return first + "=" + url_encode(second);
  }

  template <typename TKey, typename TVal>
  static std::string postPair(const std::string& name,
                              std::map<TKey, TVal> map) {
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
  static std::string postPair(const std::string& name,
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
  static std::string postPair(const std::string& name, std::vector<T> vec) {
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

  template <typename T>
  static std::string url_encode(T val) {
    return url_encode(std::to_string(val).c_str());
  }
  static std::string url_encode(std::string val) {
    return url_encode(val.c_str());
  }
  static std::string url_encode(const char* str) {
    char* encoded = curl_easy_escape(nullptr, str, 0);
    std::string result(encoded);
    curl_free(encoded);
    return result;
  }

  struct handle {
    handle() { Handle = curl_easy_init(); }
    ~handle() {
      if (Handle != nullptr) {
        curl_easy_cleanup(Handle);
      }
    }

    CURL* Handle;
  };
  typedef utility::ResourcePool<handle> handle_pool;

  handle_pool HandlePool;

  HTTP() : HandlePool(10) {}

  static long getResponseCode(CURL* h) {
    long code;
    auto err = curl_easy_getinfo(h, CURLINFO_RESPONSE_CODE, &code);
    if (err) {
      log_debugs << "CURL Error with response: " << curl_easy_strerror(err);
      return -1;
    }
    log_debugs << "HTTP Response Code: " << code;
    return code;
  }
  // Reimplement to avoid copies.
  static size_t write_data(char* buffer, size_t size, size_t nmemb,
                           std::ostream* stream) {
    stream->write(buffer, size * nmemb);
    return size * nmemb;
  }

  // Reimplement to avoid copies.
  static size_t read_data(char* buffer, size_t size, size_t nmemb,
                          std::istream* stream) {
    stream->read(buffer, size * nmemb);
    if (stream->eof()) {
      return stream->gcount();
    }
    return size * nmemb;
  }

  friend class zi::singleton<HTTP>;
};

#undef SET_OPT
}
}