#include "users/omUsersImpl.hpp"

/*static*/ const std::string om::users::defaultUser = "_default";

om::users::users(const om::file::Paths& paths) : impl_(new usersImpl(paths)) {
  SwitchToDefaultUser();
}

om::users::~users() {}

void om::users::SwitchToDefaultUser() { impl_->SwitchToDefaultUser(); }

void om::users::SwitchToUser(const std::string& userName) {
  impl_->SwitchToUser(userName);
}

QString om::users::LogFolderPath() { return impl_->LogFolderPath(); }

void om::users::SetupFolders() { impl_->SetupFolders(); }

std::string om::users::UsersFolder() { return impl_->UsersFolder(); }

std::string om::users::UsersRootFolder() { return impl_->UsersRootFolder(); }

const std::string& om::users::CurrentUser() const {
  return impl_->CurrentUser();
}

om::userSettings& om::users::UserSettings() { return impl_->UserSettings(); }
