#pragma once

#include "network/curlCore.h"
#include "network/uri.hpp"
#include "datalayer/paths.hpp"
#include "yaml-cpp/yaml.h"
#include "network/http/postStringEncoding.hpp"

namespace om {
namespace network {
namespace http {

class HTTPRequest : public Request {
 public:
  HTTPRequest(network::Uri uri) : uri_(uri) {}
  virtual void SetCurlOptions(CURL* h) override;

 private:
  PROP(network::Uri, uri);
};

class GetRequest : public HTTPRequest,
                   public virtual thread::Continuable<std::string> {
 public:
  GetRequest(network::Uri uri) : HTTPRequest(uri) {}
  virtual void SetCurlOptions(CURL* h) override;
  virtual void Finish(CURL*) { do_continuation(string()); }

  std::string string() { return ss_.str(); }

 private:
  std::stringstream ss_;
};

template <typename T>
class TypedGetRequest
    : public GetRequest,
      public virtual thread::Continuable<const boost::optional<T>&> {
 public:
  TypedGetRequest(network::Uri uri) : GetRequest(uri) {}

  virtual void Finish(CURL*) {
    parse();
    do_continuation(result_);
  }

 private:
  void parse() {
    auto response = ss_.str();
    if (response.empty()) {
      return;
    }

    try {
      auto node = YAML::Load(response);
      result_ = node.as<T>();
    }
    catch (YAML::Exception e) {
      log_debugs << "Failed loading JSON: " << e.what();
    }
  }

  PROP_REF(boost::optional<T>, result);
};

class PutRequest : public HTTPRequest, public thread::Continuable<void> {
 public:
  PutRequest(network::Uri uri) : HTTPRequest(uri) {}

 protected:
  virtual void SetCurlOptions(CURL* h) override;
  virtual void Finish(CURL* h) override { do_continuation(); }
  std::string string() { return ss_.str(); }

 protected:
  std::stringstream ss_;
};

template <typename T>
class TypedPutRequest : public PutRequest {
 public:
  TypedPutRequest(network::Uri uri, T&& data)
      : PutRequest(uri), data_(std::move(data)) {}

 protected:
  virtual void SetCurlOptions(CURL* h) override {
    if (!toJson()) {
      throw InvalidOperationException(std::string("Unable to put data to: ") +
                                      uri_.string());
    }
    PutRequest::SetCurlOptions(h);
  }

  bool toJson() {
    try {
      YAML::Emitter e;
      e.SetSeqFormat(YAML::Flow);
      e.SetMapFormat(YAML::Flow);
      e << data_;
      ss_.write(e.c_str(), e.size());
      return true;
    }
    catch (YAML::Exception e) {
      log_debugs << "Failed converting to JSON: " << e.what();
      return false;
    }
  }

  PROP_REF(T, data);
};

class PostRequest
    : public HTTPRequest,
      public thread::Continuable<const boost::optional<std::string>&> {
 public:
  template <typename... TRest>
  PostRequest(network::Uri uri, TRest&&... rest)
      : HTTPRequest(uri), postString_(postString(rest...)) {}

 protected:
  virtual void SetCurlOptions(CURL* h) override;
  virtual void Finish(CURL* h) override;

  std::stringstream ss_;
  PROP_REF(std::string, postString);
  PROP_REF(boost::optional<std::string>, result);
};
}
}
}  // namespace om::network::

#undef SET_OPT