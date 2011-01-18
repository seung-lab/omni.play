#include "system/omProjectData.h"
#include "project/omProject.h"
#include "project/omProjectDataImpl.hpp"

OmProjectData::OmProjectData()
{}

void OmProjectData::instantiateProjectData(const std::string& fileNameAndPath ){
	instance().impl_->instantiateProjectData(fileNameAndPath);
}

void OmProjectData::Create(){
	instance().impl_ = boost::make_shared<OmProjectDataImpl>();
}

void OmProjectData::Delete(){
	instance().impl_.reset();
}

QString OmProjectData::getFileNameAndPath(){
	return OmProject::GetFileNameAndPath();
}

QString OmProjectData::getAbsoluteFileNameAndPath(){
	return OmProject::GetFileNameAndPath();
}

QString OmProjectData::getAbsolutePath(){
	return OmProject::GetPath();
}

const QDir& OmProjectData::GetFilesFolderPath(){
	return instance().impl_->GetFilesFolderPath();
}

void OmProjectData::CreateProject(){
	instance().impl_->CreateProject();
}

void OmProjectData::Open(){
	instance().impl_->Open();
}

void OmProjectData::Close(){
	instance().impl_->Close();
}

void OmProjectData::DeleteInternalData(const OmDataPath & path){
	instance().impl_->DeleteInternalData(path);
}

OmIDataReader* OmProjectData::GetProjectIDataReader(){
	return instance().impl_->GetProjectIDataReader();
}

OmIDataWriter* OmProjectData::GetIDataWriter(){
	return instance().impl_->GetIDataWriter();
}

void OmProjectData::setFileVersion(const int fileVersion){
	instance().impl_->setFileVersion(fileVersion);
}

bool OmProjectData::IsOpen(){
	return instance().impl_->IsOpen();
}

bool OmProjectData::IsReadOnly(){
	return instance().impl_->IsReadOnly();
}

int OmProjectData::getFileVersion(){
	return instance().impl_->getFileVersion();
}
