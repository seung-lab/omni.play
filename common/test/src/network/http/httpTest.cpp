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
  auto val = HTTP::GET("http://curl.haxx.se/libcurl");
  ASSERT_FALSE(val.empty());
}

TEST(HTTP, Put) {}
}
}