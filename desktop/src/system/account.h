#pragma once
#include "precomp.h"

#include "zi/utility.h"
#include "network/uri.hpp"
#include "threads/waitable.hpp"
#include "threads/continuable.hpp"

namespace om {
namespace network {
namespace http {
class PostRequest;
}
}
namespace system {

enum class LoginResult {
  PENDING,
  CANCELLED,
  SUCCESS,
  BAD_USERNAME_PW,
  CONNECTION_ERROR,
};

class LoginRequest : public thread::WaitablePassthrough,
                     public thread::Continuable<LoginResult> {
 public:
  LoginRequest(network::Uri endpoint, std::string username,
               std::shared_ptr<network::http::PostRequest> request);
  LoginRequest(LoginResult result);

 private:
  std::shared_ptr<network::http::PostRequest> request_;
  PROP_REF(network::Uri, endpoint);
  PROP_REF(std::string, username);
  PROP_REF(LoginResult, result);
};

class Account : private om::SingletonBase<Account> {
 public:
  static const std::string& username() { return instance().username_; }
  static uint userid() { return instance().userid_; }
  static bool IsLoggedIn();
  static LoginRequest Login(const std::string& username,
                            const std::string& password);

  static network::Uri endpoint();
  static network::Uri endpoint(const std::string& path);
  static void set_endpoint(network::Uri endpoint);

 private:
  static LoginResult parseLoginResult(std::shared_ptr<std::string> str,
                                      const std::string& username);
  Account();
  ~Account();

  std::string username_;
  uint userid_;
  network::Uri endpoint_;

  friend class zi::singleton<Account>;
  friend class LoginRequest;
};
}
}  // namespace om::system::