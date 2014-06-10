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

LoginRequest Account::Login(const std::string& username,
                            const std::string& password) {
  if (!task::TaskManager::AttemptFinishTask()) {
    return LoginRequest(LoginResult::CANCELLED);
  }
  auto login = instance().endpoint_;
  login.set_path("/1.0/internal/account/authenticate/standard");
  auto req = network::http::POST(login, std::make_pair("username", username),
                                 std::make_pair("password", password));

  return LoginRequest(instance().endpoint_, username, req);
}

network::Uri Account::endpoint() { return instance().endpoint_; }
network::Uri Account::endpoint(const std::string& path) {
  auto uri = instance().endpoint_;
  uri.set_path(path);
  return uri;
}
void Account::set_endpoint(network::Uri endpoint) {
  instance().endpoint_ = endpoint;
  instance().endpoint_.set_scheme("http");
}

LoginResult Account::parseLoginResult(std::shared_ptr<std::string> result,
                                      const std::string& username) {
  if (!result) {
    om::event::ConnectionChanged();
    return LoginResult::CONNECTION_ERROR;
  }
  try {
    auto node = YAML::Load(*result);
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

LoginRequest::LoginRequest(network::Uri endpoint, std::string username,
                           std::shared_ptr<network::http::PostRequest> request)
    : WaitablePassthrough(request.get()),
      request_(request),
      endpoint_(endpoint),
      username_(username),
      result_(LoginResult::PENDING) {
  request->AddContinuation([this](std::shared_ptr<std::string> str) {
    result_ = Account::parseLoginResult(str, username_);
    do_continuation(result_);
  });
}

LoginRequest::LoginRequest(LoginResult result) : result_(result) {}
}
}  // namespace om::system::