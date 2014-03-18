#pragma once

#include "precomp.h"
#include "common/macro.hpp"
#include "common/logging.h"
#include "utility/resourcePool.hpp"
#include "datalayer/dataSource.hpp"
#include "network/http/typedefs.hpp"
#include "network/http/interface.hpp"

namespace om {
namespace datalayer {

#define SET_OPT(h, opt, val, bad_ret)                                  \
  err = curl_easy_setopt(h, opt, val);                                 \
  if (err != CURLE_OK) {                                               \
    log_debugs << "Failed to set " << #opt << curl_easy_strerror(err); \
    return bad_ret;                                                    \
  }

template <typename TKey, typename TValue,
          typename APIVersionTag = network::http::V1,
          typename DataVersionTag = network::http::V1, typename... Policies>
class ClientDS : public IDataSource<TKey, TValue> {
  typedef network::http::interface<TValue, DataVersionTag> http_interface;

 public:
  ClientDS(std::string endpoint) : endpoint_(endpoint), handlePool_(100) {}
  virtual ~ClientDS() {}

  virtual std::shared_ptr<TValue> Get(const TKey& key,
                                      bool async = false) override {
    typename handle_pool::Lease h(handlePool_);
    if (!h) {
      log_debugs << "Out of connections!";
      return std::shared_ptr<TValue>();
    }
    auto p = network::http::path(APIVersionTag(), endpoint_, key);
    CURLcode err;

    SET_OPT(h->Handle, CURLOPT_URL, p.c_str(), std::shared_ptr<TValue>());
    SET_OPT(h->Handle, CURLOPT_FOLLOWLOCATION, 1, std::shared_ptr<TValue>());
    SET_OPT(h->Handle, CURLOPT_WRITEFUNCTION, &write_data,
            std::shared_ptr<TValue>());
    std::stringstream ss;
    SET_OPT(h->Handle, CURLOPT_WRITEDATA, &ss, std::shared_ptr<TValue>());

    if (!call_all(Policies::add_get_options(h->Handle, key)...)) {
      return std::shared_ptr<TValue>();
    }

    err = curl_easy_perform(h->Handle);
    if (err) {
      log_debugs << "CURL Error getting: " << key << " "
                 << curl_easy_strerror(err);
      return std::shared_ptr<TValue>();
    }

    long code;
    err = curl_easy_getinfo(h->Handle, CURLINFO_RESPONSE_CODE, &code);
    if (err) {
      log_debugs << "CURL Error with response: " << key
                 << curl_easy_strerror(err);
      return std::shared_ptr<TValue>();
    }
    log_debugs << "HTTP Response Code: " << code;

    auto str = ss.str();
    log_debugs << "HTTP Response: " << str;
    return http_interface::deserialize(key, str);
  }

  virtual bool Put(const TKey& key, std::shared_ptr<TValue> value,
                   bool async = false) override {
    typename handle_pool::Lease h(handlePool_);
    if (!h) {
      log_debugs << "Out of connections!";
      return false;
    }
    auto p = network::http::path(APIVersionTag(), endpoint_, key);
    CURLcode err;
    SET_OPT(h->Handle, CURLOPT_URL, p.c_str(), false);
    SET_OPT(h->Handle, CURLOPT_FOLLOWLOCATION, 1, false);
    SET_OPT(h->Handle, CURLOPT_READFUNCTION, &read_data, false);
    std::stringstream ss(http_interface::serialize(value));
    SET_OPT(h->Handle, CURLOPT_READDATA, &ss, false);
    SET_OPT(h->Handle, CURLOPT_UPLOAD, 1, false);

    if (!call_all(Policies::add_put_options(h->Handle, key, value)...)) {
      return false;
    }

    err = curl_easy_perform(h->Handle);
    if (err) {
      log_debugs << "CURL Error uploading: " << key << curl_easy_strerror(err);
      return false;
    }
    return true;
  }

  virtual void Flush() override {}
  virtual bool is_cache() const override { return false; }
  virtual bool is_persisted() const override { return true; }

 private:
  template <typename... Types>
  bool call_all(CURLcode err, Types... t) {
    if (!call_all(err)) {
      return false;
    }
    return call_all(t...);
  }
  bool call_all(CURLcode err) {
    if (err) {
      log_debugs << "CURL Error: " << curl_easy_strerror(err);
      return false;
    }
    return true;
  }
  bool call_all() { return true; }

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

  PROP_REF_SET(std::string, endpoint);
  handle_pool handlePool_;
};

#undef SET_OPT
}
}  // namespace datalayer::