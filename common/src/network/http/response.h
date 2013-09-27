#pragma once

#include "common/macro.hpp"
#include "network/http/typedefs.hpp"

namespace om {
namespace network {
namespace http {
class Response {
 public:

  Response(client::response response);
  Response();

  bool Success() const;
  operator client::response() const;

 protected:
  PROP_REF(std::string, allow);
  PROP_REF(std::string, contentEncoding);
  PROP_REF(std::string, contentLength);
  PROP_REF(std::string, contentRange);
  PROP_REF(std::string, contentType);
  PROP_REF(std::string, expires);
  PROP_REF(std::string, lastModified);
  PROP_REF(std::string, setCookie);
  PROP_REF(std::string, status);
  PROP_REF(std::string, source);
  PROP_REF(std::string, destination);
  PROP_REF(std::string, body);
};
}
}
}  // namespace om::network::