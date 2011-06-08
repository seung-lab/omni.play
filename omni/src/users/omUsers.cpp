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

QString om::users::GetVolSegmentsPathAbs(OmSegmentation* vol){
    return impl_->GetVolSegmentsPathAbs(vol);
}

QString om::users::LogFolderPath(){
    return impl_->LogFolderPath();
}

void om::users::SetupFolders(){
    impl_->SetupFolders();
}
