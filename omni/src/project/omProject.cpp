#include "project/omProject.h"
#include "project/omProjectImpl.hpp"

QString OmProject::New(const QString& fnp)
{
	instance().impl_ = boost::make_shared<OmProjectImpl>();
	return instance().impl_->New(fnp);
}

void OmProject::Save(){
	instance().impl_->Save();
}

void OmProject::Commit(){
	instance().impl_->Commit();
}

void OmProject::Load(const QString& fnp)
{
	instance().impl_ = boost::make_shared<OmProjectImpl>();
	return instance().impl_->Load(fnp);
}

const QString& OmProject::FilesFolder(){
	return instance().impl_->FilesFolder();
}

const QString& OmProject::OmniFile(){
	return instance().impl_->OmniFile();
}

bool OmProject::HasOldHDF5(){
	return instance().impl_->HasOldHDF5();
}

OmHdf5* OmProject::OldHDF5(){
	return instance().impl_->OldHDF5();
}

OmProjectVolumes& OmProject::Volumes(){
	return instance().impl_->Volumes();
}

int OmProject::GetFileVersion(){
	return instance().impl_->GetFileVersion();
}

void OmProject::setFileVersion(const int ver){
	instance().impl_->setFileVersion(ver);
}

bool OmProject::IsReadOnly(){
	return instance().impl_->IsReadOnly();
}

OmProjectGlobals& OmProject::Globals(){
	return instance().impl_->Globals();
}

bool OmProject::IsOpen(){
	return instance().impl_;
}

#include "actions/omActions.h"
#include "segment/omSegmentSelected.hpp"
#include "system/events/omEventManager.h"
#include "system/omGarbage.h"
#include "utility/omThreadPoolManager.h"
#include "zi/omThreads.h"

void OmProject::Close()
{
	if(!IsOpen()){
		return;
	}

	OmActions::Close();

	OmCacheManager::SignalCachesToCloseDown();
	OmThreadPoolManager::StopAll();
	zi::all_threads::join_all();

	// OmProject must be deleted here, remaining singletons close cleanly
	instance().impl_.reset();

	//delete all singletons
	OmSegmentSelected::Delete();
	OmEventManager::Delete();
	OmGarbage::Delete();
	OmPreferences::Delete();
	//OmLocalPreferences

	//close project data
	OmTileCache::Delete();
	OmCacheManager::Delete();

	OmHdf5Manager::Delete();
}
