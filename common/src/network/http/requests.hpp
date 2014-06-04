#pragma once

#include "network/curlCore.h"
#include "network/uri.hpp"
#include "datalayer/paths.hpp"
#include "yaml-cpp/yaml.h"

#define SET_OPT(h, opt, val)                                           \
  if ((auto err = curl_easy_setopt(h, opt, val)) != CURLE_OK) {        \
    log_debugs << "Failed to set " << #opt << curl_easy_strerror(err); \
    return;                                                            \
  }

namespace om {
namespace network {
namespace http {

#include "requests_internal.hpp"

template <typename T>
class GetRequest : public Request,
                   public thread::Continuable<std::shared_ptr<T>> {
 public:
  GetRequest(network::Uri uri) : uri_(uri) {}
  ~GetRequest();

 protected:
  virtual void SetCurlOptions(CURL* h) override {
    internal::GetOptions(h, uri_, &ss_);
  }

  virtual void Finish(CURL* h) override {
    parse();
    do_continuation(result_);
  }

  void parse() {
    auto response = ss_.str();
    if (response.empty()) {
      result_ = std::shared_ptr<T>();
      return;
    }

    std::shared_ptr<T> val;
    try {
      auto node = YAML::Load(response);
      val.reset(new T(node.as<T>()));
    }
    catch (YAML::Exception e) {
      log_debugs << "Failed loading JSON: " << e.what();
    }
    result_ = val;
  }

  std::stringstream ss_;
  PROP(network::Uri, uri);
  PROP(std::shared_ptr<T>, result);
};

template <>
class GetRequest<std::string> : public Request,
                                public thread::Continuable<
                                    std::shared_ptr<std::string>> {
 public:
  GetRequest(network::Uri uri) : uri_(uri) {}
  ~GetRequest();

 protected:
  virtual void SetCurlOptions(CURL* h) override {
    internal::GetOptions(h, uri_, &ss_);
  }
  virtual void Finish(CURL* h) override { do_continuation(result_); }

  std::stringstream ss_;
  PROP(network::Uri, uri);
  PROP(std::shared_ptr<std::string>, result);
};

template <typename T>
class PutRequest : public Request, public thread::Continuable<void> {
 public:
  PutRequest(network::Uri uri, T&& data) : uri_(uri), data_(std::move(data)) {}
  ~PutRequest();

 protected:
  virtual void SetCurlOptions(CURL* h) override {
    if (!toJson()) {
      log_debugs << "Unable to put data to: " << uri_;
    }
    internal::PutOptions(h, uri_, &ss_);
  }
  virtual void Finish(CURL* h) override { do_continuation(); }

  bool toJson() {
    try {
      YAML::Emitter e;
      e.SetSeqFormat(YAML::Flow);
      e.SetMapFormat(YAML::Flow);
      e << val;
      ss_.write(e.c_str(), e.size());
      return true;
    }
    catch (YAML::Exception e) {
      log_debugs << "Failed converting to JSON: " << e.what();
      return false;
    }
  }

  std::stringstream ss_;
  PROP(network::Uri, uri);
  PROP_REF(T, data_);
};

template <>
class PutRequest : public Request, public thread::Continuable<void> {
 public:
  PutRequest(network::Uri uri, const std::string& data)
      : uri_(uri), ss_(data) {}
  ~PutRequest();

 protected:
  virtual void SetCurlOptions(CURL* h) override {
    internal::PutOptions(h, uri_, &ss_);
  }
  virtual void Finish(CURL* h) override { do_continuation(); }

  std::stringstream ss_;
  PROP(network::Uri, uri);
  PROP_REF(std::string, data_);
};

template <typename... TRest>
class PostRequest : public Request, public thread::Continuable<void> {
 public:
  PostRequest(network::Uri uri, TRest&&... rest)
      : uri_(uri), postString_(internal::postString(rest...)) {}
  ~PostRequest();

 protected:
  virtual void SetCurlOptions(CURL* h) override {
    internal::PostOptions(h, uri_, &ss_, postString_);
  }
  virtual void Finish(CURL* h) override { do_continuation(); }

  std::stringstream ss_;
  PROP(network::Uri, uri);
  PROP_REF(std::string, postString_);
};
}
}
}  // namespace om::network::

#undef SET_OPT