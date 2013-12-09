#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "datalayer/client.hpp"
#include <thread>

using ::testing::DefaultValue;
using ::testing::_;
using namespace om::datalayer;

namespace om {
namespace network {
namespace http {
template <typename Tag> std::string partial_path(Tag, const std::string& key) {
  return key;
}
}
}

namespace test {

TEST(Datalayer_Client, Get) {
  ClientDS<std::string, std::string> testClient("http://curl.haxx.se");
  auto val = testClient.Get("libcurl");
  ASSERT_TRUE((bool) val);
}

TEST(Datalayer_Client, Put) {}
}
}