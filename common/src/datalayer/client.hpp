#pragma once

#include "common/macro.hpp"
#include "common/logging.h"
#include "utility/resourcePool.hpp"
#include "datalayer/dataSource.hpp"
#include "network/http/typedefs.hpp"
#include "network/http/interface.hpp"

#include <curl/curl.h>

namespace om {
namespace datalayer {
template <typename TKey, typename TValue,
          typename APIVersionTag = network::http::V1,
          typename DataVersionTag = network::http::V1>
class ClientDS : public IDataSource<TKey, TValue> {
  typedef network::http::interface<TValue, DataVersionTag> http_interface;

 public:
  ClientDS(std::string endpoint) : endpoint_(endpoint), handlePool_(100) {}
  virtual ~ClientDS() {}

  virtual std::shared_ptr<TValue> Get(const TKey& key, bool async = false)
      override {
    typename handle_pool::Lease h(handlePool_);
    if (!h) {
      log_debugs(HTTP) << "Out of connections!";
      return std::shared_ptr<TValue>();
    }
    auto p = network::http::path(APIVersionTag(), endpoint_, key);
    auto err = curl_easy_setopt(h->Handle, CURLOPT_URL, p.c_str());
    if (err != CURLE_OK) {
      log_errors(HTTP) << "Failed to set url: " << p << " "
                       << curl_easy_strerror(err);
      return false;
    }

    err = curl_easy_setopt(h->Handle, CURLOPT_WRITEFUNCTION, &write_data);
    if (err != CURLE_OK) {
      log_errors(HTTP)
          << "Failed to set write callback: " << curl_easy_strerror(err);
      return false;
    }

    std::stringstream ss;
    err = curl_easy_setopt(h->Handle, CURLOPT_WRITEDATA, &ss);
    if (err != CURLE_OK) {
      log_errors(HTTP)
          << "Failed to set write data: " << curl_easy_strerror(err);
      return false;
    }

    err = curl_easy_perform(h->Handle);
    if (err) {
      log_errors(HTTP) << "CURL Error getting: " << key
                       << curl_easy_strerror(err);
      return std::shared_ptr<TValue>();
    }
    return http_interface::deserialize(key, ss.str());
  }

  virtual bool Put(const TKey& key, std::shared_ptr<TValue> value,
                   bool async = false) override {
    typename handle_pool::Lease h(handlePool_);
    if (!h) {
      log_debugs(HTTP) << "Out of connections!";
      return false;
    }
    auto p = network::http::path(APIVersionTag(), endpoint_, key);
    auto err = curl_easy_setopt(h->Handle, CURLOPT_URL, p.c_str());
    if (err != CURLE_OK) {
      log_errors(HTTP) << "Failed to set url: " << p << " "
                       << curl_easy_strerror(err);
      return false;
    }

    err = curl_easy_setopt(h->Handle, CURLOPT_READFUNCTION, &read_data);
    if (err != CURLE_OK) {
      log_errors(HTTP)
          << "Failed to set read callback: " << curl_easy_strerror(err);
      return false;
    }

    std::stringstream ss(http_interface::serialize(value));
    err = curl_easy_setopt(h->Handle, CURLOPT_READDATA, &ss);
    if (err != CURLE_OK) {
      log_errors(HTTP)
          << "Failed to set read data: " << curl_easy_strerror(err);
      return false;
    }

    err = curl_easy_setopt(h->Handle, CURLOPT_UPLOAD, 1);
    if (err != CURLE_OK) {
      log_errors(HTTP)
          << "Failed to set upload option: " << curl_easy_strerror(err);
      return false;
    }

    err = curl_easy_perform(h->Handle);
    if (err) {
      log_errors(HTTP) << "CURL Error putting: " << key
                       << curl_easy_strerror(err);
      return false;
    }
    return true;
  }

  virtual void Flush() override {}
  virtual bool is_cache() const override { return false; }
  virtual bool is_persisted() const override { return true; }

 private:
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

  PROP_CONST_REF(std::string, endpoint);
  handle_pool handlePool_;
};
}
}  // namespace datalayer::