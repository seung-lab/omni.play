#pragma once

#include <boost/network/include/http/client.hpp>

namespace om {
namespace network {
using boost::network::uri::uri;
namespace http {
typedef boost::network::http::basic_client<
    boost::network::http::tags::http_default_8bit_udp_resolve, 1, 1> client;
}
}
}  // namespace om::network::