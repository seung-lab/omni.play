#include "system/account.h"
#include "network/http/http.hpp"
#include "yaml-cpp/yaml.h"
#include "system/omLocalPreferences.hpp"
#include "events/events.h"
#include "task/taskManager.h"

namespace om {
namespace system {

Account::Account()
    : username_(OmLocalPreferences::getUsername().toStdString()),
      endpoint_(OmLocalPreferences::getEndpoint().toStdString()) {}
Account::~Account() {}

bool Account::IsLoggedIn() {
  return instance().userid_ > 0 && !instance().username_.empty();
}

Account::LoginResult Account::Login(const std::string& username,
                                    const std::string& password) {
  if (!task::TaskManager::FinishTask()) {
    return LoginResult::CANCELLED;
  }
  std::string endpoint = instance().endpoint_;
  endpoint += "/1.0/internal/account/authenticate/standard";
  auto str = network::HTTP::POST(endpoint, std::make_pair("username", username),
                                 std::make_pair("password", password));

  instance().username_ = "";
  instance().userid_ = 0;
  if (!str) {
    om::event::ConnectionChanged();
    return LoginResult::CONNECTION_ERROR;
  }
  try {
    auto node = YAML::Load(str.get());
    if (!node["success"].as<bool>(false)) {
      om::event::ConnectionChanged();
      return LoginResult::BAD_USERNAME_PW;
    }

    instance().username_ = username;
    instance().userid_ = node["user"]["id"].as<uint>(0);
    om::event::ConnectionChanged();
    return LoginResult::SUCCESS;
  }
  catch (YAML::Exception e) {
    log_debugs << "Failed logging in: " << e.what();
    om::event::ConnectionChanged();
    return LoginResult::CONNECTION_ERROR;
  }
}

const std::string& Account::endpoint() { return instance().endpoint_; }
void Account::set_endpoint(const std::string& endpoint) {
  instance().endpoint_ = endpoint;
}
}
}  // namespace om::system::