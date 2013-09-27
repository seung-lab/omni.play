// #include "gtest/gtest.h"
// #include "gmock/gmock.h"

// #include "datalayer/client.hpp"
// #include <boost/network/include/http/server.hpp>
// #include <thread>

// using ::testing::DefaultValue;
// using ::testing::_;
// using namespace boost::network::http;
// using namespace om::datalayer;

// namespace om {
// namespace network {
// namespace http {
// uri partial_path(const std::string& key) { return uri(key); }

// template <>
// struct interface<std::string> {
//   static std::string mime_type() { return "text/plain"; }
//   static std::string serialize(std::shared_ptr<std::string> data) {
//     return *data;
//   }
//   template <typename TKey>
//   static std::shared_ptr<std::string> deserialize(const TKey& key,
//                                                   const std::string& data) {
//     return std::make_shared<std::string>(data);
//   }
// };
// }
// }

// namespace test {

// struct handler;
// typedef server<handler> http_server;

// struct handler {
//   void operator()(http_server::request const& req, http_server::response&
// res) {
//     std::string dest = destination(req);
//     // res << body("test: " + dest);
//   }
// };

// class Datalayer_Client : public ::testing::Test {
//  public:
//   Datalayer_Client() {
//     // Start Test Server
//     async_server<handler>::options options(serverHandler_);
//     options.address("127.0.0.1").port("80").reuse_address(true);
//     testServer_.reset(new http_server(options));
//     boost::thread t1(boost::bind(&http_server::run, testServer_.get()));

//     // Initialize Client
//     client_.reset(new ClientDS(network::uri("http://localhost/v1")));
//   }

//  protected:
//   handler serverHandler_;
//   std::unique_ptr<http_server> testServer_;
//   std::thread serverThread_;
//   std::unique_ptr<ClientDS<std::string, std::string>> client_;
// };

// TEST_F(Datalayer_Client, Get) {
//   ASSERT_EQ("test: http://localhost/v1/foo", client_->Get("foo"));
// }

// TEST_F(Datalayer_Client, Put) {}
// }
// }