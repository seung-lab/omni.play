#include "network/http/response.h"

using namespace boost::network;
using namespace boost::network::http;

#define READ_HEADER(name, value) \
  h = hs.find(name);             \
  if (h != hs.end()) {           \
    value = h->second;           \
  }

namespace om {
namespace network {
namespace http {

Response::Response(client::response response) {
  client::response::headers_container_type hs = headers(response);
  client::response::headers_container_type::iterator h;
  READ_HEADER("Allow", allow_);
  READ_HEADER("Content-Encoding", contentEncoding_);
  READ_HEADER("Content-Length", contentLength_);
  READ_HEADER("Content-Range", contentRange_);
  READ_HEADER("Content-Type", contentType_);
  READ_HEADER("Expires", expires_);
  READ_HEADER("Last-Modified", lastModified_);
  READ_HEADER("Set-Cookie", setCookie_);
  READ_HEADER("Status", status_);
  body_ = response.body();
  source_ = response.source();
  destination_ = response.destination();
}

Response::Response() {}

bool Response::Success() const {
  return status_.find("200") != std::string::npos;
}

Response::operator client::response() const {
  client::response r;
  add_header(r, "Allow", allow_);
  add_header(r, "Content-Encoding", contentEncoding_);
  add_header(r, "Content-Length", contentLength_);
  add_header(r, "Content-Range", contentRange_);
  add_header(r, "Content-Type", contentType_);
  add_header(r, "Expires", expires_);
  add_header(r, "Last-Modified", lastModified_);
  add_header(r, "Set-Cookie", setCookie_);
  add_header(r, "Status", status_);
  boost::network::body(r, body_);
  boost::network::body(r, body_);
  boost::network::body(r, body_);
  return r;
}
}
}
}
#undef READ_HEADER