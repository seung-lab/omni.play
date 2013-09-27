#pragma once

#include "common/logging.h"
#include "datalayer/dataSource.hpp"
#include "network/http/typedefs.hpp"
#include "network/http/request.h"
#include "network/http/response.h"
#include "network/http/interface.hpp"

#include <mutex>

namespace om {
namespace datalayer {
template <typename TKey, typename TValue,
          typename APIVersionTag = network::http::V1,
          typename DataVersionTag = network::http::V1>
class ClientDS : public IDataSource<TKey, TValue> {
  typedef network::http::interface<TValue, DataVersionTag> http_interface;

 public:
  ClientDS(network::uri endpoint) : endpoint_(endpoint) {
    network::http::client::options options;
    options.follow_redirects(true).cache_resolved(true);
    client_.reset(new network::http::client(options));
  }

  virtual ~ClientDS() {}
  virtual std::shared_ptr<TValue> Get(const TKey& key,
                                      bool async = false) override {
    network::http::Request request(
        network::http::path(APIVersionTag(), endpoint_, key));
    request.accept() = http_interface::mime_type();
    try {
      network::http::client::response rawResponse;
      {
        std::lock_guard<std::mutex> g(mutex_);
        rawResponse = client_->get(request);
      }

      network::http::Response response(rawResponse);
      if (!response.Success()) {
        return std::shared_ptr<TValue>();
      }
      return http_interface::deserialize(key, response.body());
    }
    catch (std::exception e) {
      log_errors(unknown) << "Error receiving " << key << ": " << e.what();
      return std::shared_ptr<TValue>();
    }
  }

  virtual bool Put(const TKey& key, std::shared_ptr<TValue> value,
                   bool async = false) override {
    network::http::Request request(
        network::http::path(APIVersionTag(), endpoint_, key));
    request.body() = http_interface::serialize(value);
    request.contentType() = http_interface::mime_type();
    if (async) {
      // asnycClient_.put(r);  // Deal with futures.
    } else {
      try {
        network::http::client::response rawResponse;
        {
          std::lock_guard<std::mutex> g(mutex_);
          rawResponse = client_->put(request);
        }
        network::http::Response response(rawResponse);
        return response.Success();
      }
      catch (std::exception e) {
        log_errors(unknown) << "Error sending " << key << ": " << e.what();
        return false;
      }
    }
    return true;
  }

  virtual void Flush() override {}
  virtual bool is_cache() const override { return false; }
  virtual bool is_persisted() const override { return true; }

 private:
  std::unique_ptr<network::http::client> client_;
  // http::client asyncClient_;
  std::mutex mutex_;
  PROP_CONST_REF(network::uri, endpoint);
};
}
}  // namespace datalayer::