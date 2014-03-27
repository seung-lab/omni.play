#include "users/omUsers.h"
#include "users/omUsersImpl.hpp"

/*static*/ const std::string om::users::defaultUser = "_default";

om::users::users(om::file::Paths paths) : impl_(new usersImpl(paths)) {
  SwitchToDefaultUser();
}

om::users::~users() {}

void om::users::SwitchToDefaultUser() { impl_->SwitchToDefaultUser(); }

void om::users::SwitchToUser(const std::string& userName) {
  impl_->SwitchToUser(userName);
}

QString om::users::LogFolderPath() {
  return impl_->UserPaths().LogFiles().c_str();
}
void om::users::SetupFolders() { impl_->SetupFolders(); }

om::file::Paths::Usr om::users::UserPaths() { return impl_->UserPaths(); }

const std::string& om::users::CurrentUser() const {
  return impl_->CurrentUser();
}

om::userSettings& om::users::UserSettings() { return impl_->UserSettings(); }
