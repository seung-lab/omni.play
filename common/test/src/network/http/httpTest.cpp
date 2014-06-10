#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "network/http/http.hpp"
#include <thread>

using ::testing::DefaultValue;
using ::testing::_;
using namespace om::network;

namespace om {
namespace test {

TEST(HTTP, GET) {
  auto val = http::GET<std::string>("http://curl.haxx.se/libcurl"_uri);
  val->wait();
  ASSERT_TRUE((bool)*val);
}

TEST(HTTP, Put) {}
}
}