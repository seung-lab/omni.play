#pragma once
#include "precomp.h"

#include "zi/utility.h"
#include "network/uri.hpp"

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

  static network::Uri endpoint();
  static network::Uri endpoint(const std::string& path);
  static void set_endpoint(network::Uri endpoint);

 private:
  Account();
  ~Account();

  std::string username_;
  uint userid_;
  network::Uri endpoint_;

  friend class zi::singleton<Account>;
};
}
}  // namespace om::system::