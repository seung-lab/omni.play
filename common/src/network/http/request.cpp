#include "network/http/request.h"
#include <boost/network/message.hpp>
#include <boost/network/message/wrappers.hpp>
#include <boost/network/uri.hpp>

#define READ_HEADER(name, value) \
  h = hs.find(name);             \
  if (h != hs.end()) {           \
    value = h->second;           \
  }

using namespace boost::network;
using namespace boost::network::http;

namespace om {
namespace network {
namespace http {

Request::Request(const client::request& request) {
  client::request::headers_container_type hs = headers(request);
  client::request::headers_container_type::iterator h;
  READ_HEADER("Accept", accept_);
  READ_HEADER("Authorization", authorization_);
  READ_HEADER("Content-Type", contentType_);
  READ_HEADER("Range", range_);
  destination_ = request.destination();
  body_ = request.body();
}
Request::Request() {}

Request::operator client::request() const {
  client::request r;
  add_header(r, "Accept", accept_);
  add_header(r, "Authorization", authorization_);
  add_header(r, "Content-Type", contentType_);
  add_header(r, "Range", range_);
  boost::network::destination(r, destination_);
  boost::network::body(r, body_);
  return r;
}
}
}
}
#undef READ_HEADER