#pragma once

#include "zi/utility.h"
#include <string>
#include <memory>
#include "network/http/httpScope.hpp"

namespace om {
namespace system {

class Account : private om::SingletonBase<Account> {
 public:
  enum class LoginResult {
    CANCELLED,
    SUCCESS,
    BAD_USERNAME_PW,
    CONNECTION_ERROR,
  };
  static const std::string& username() { return instance().username_; }
  static uint userid() { return instance().userid_; }
  static bool IsLoggedIn();
  static LoginResult Login(const std::string& username,
                           const std::string& password);

  static const std::string& endpoint();
  static void set_endpoint(const std::string& endpoint);

  static network::HTTPScope& Scope() { return instance().scope_; }

 private:
  Account();
  ~Account();

  std::string username_;
  uint userid_;
  std::string endpoint_;
  network::HTTPScope scope_;

  friend class zi::singleton<Account>;
};
}
}  // namespace om::system::