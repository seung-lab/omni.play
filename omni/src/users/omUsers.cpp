#include "users/omUsersImpl.hpp"

om::users::users()
    : impl_(new usersImpl())
{
    SwitchToDefaultUser();
}

om::users::~users()
{}

void om::users::SwitchToDefaultUser(){
    impl_->SwitchToDefaultUser();
}

void om::users::SwitchToUser(const std::string& userName){
    impl_->SwitchToUser(userName);
}

QString om::users::LogFolderPath(){
    return impl_->LogFolderPath();
}

void om::users::SetupFolders(){
    impl_->SetupFolders();
}

std::string om::users::UsersFolder(){
    return impl_->UsersFolder();
}

std::string om::users::UsersRootFolder(){
    return impl_->UsersRootFolder();
}
