#pragma once

#include "common/macro.hpp"
#include "network/http/typedefs.hpp"

namespace om {
namespace network {
namespace http {
class Request {
 public:
  Request(const client::request& request);
  Request(uri uri);
  Request();

  operator client::request() const;

 protected:
  PROP_REF(std::string, body);
  PROP_REF(std::string, destination);
  PROP_REF(std::string, accept);
  PROP_REF(std::string, authorization);
  PROP_REF(std::string, contentType);
  PROP_REF(std::string, range);
};
}
}
}  // namespace om::network::