#pragma once

#include "datalayer/paths.hpp"

namespace om {
namespace datalayer {

template <typename TKey, typename TValue> struct UseCookies {
  static CURLcode add_get_options(CURL* handle, const TKey&) {
    return curl_easy_setopt(handle, CURLOPT_COOKIEJAR,
                            file::Paths::CookieFile().c_str());
  }
  static CURLcode add_put_options(CURL* handle, const TKey&,
                                  std::shared_ptr<TValue>) {
    return curl_easy_setopt(handle, CURLOPT_COOKIEJAR,
                            file::Paths::CookieFile().c_str());
  }
};

template <typename TKey, typename TValue> struct SSLPolicy {
  static CURLcode add_get_options(CURL* handle, const TKey&) {
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 1);
    return curl_easy_setopt(handle, CURLOPT_CAINFO, "cacert.pem");
  }

  static CURLcode add_put_options(CURL* handle, const TKey&,
                                  std::shared_ptr<TValue>) {
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 1);
    return curl_easy_setopt(handle, CURLOPT_CAINFO, "cacert.pem");
  }
};

template <typename TKey, typename TValue> struct NoSSLPolicy {
  static CURLcode add_get_options(CURL* handle, const TKey&) {
    return curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
  }

  static CURLcode add_put_options(CURL* handle, const TKey&,
                                  std::shared_ptr<TValue>) {
    return curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
  }
};
}
}  // namespace om::datalayer::