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

template <> struct interface<std::string> {
  static std::string mime_type() { return "text/plain"; }
  static std::string serialize(std::shared_ptr<std::string> data) {
    return *data;
  }
  template <typename TKey>
  static std::shared_ptr<std::string> deserialize(const TKey& key,
                                                  const std::string& data) {
    return std::make_shared<std::string>(data);
  }
};
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