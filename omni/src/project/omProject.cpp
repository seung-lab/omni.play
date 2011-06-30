#include "tiles/pools/omTilePools.hpp"
#include "project/omProject.h"
#include "project/omProjectImpl.hpp"

OmProject::OmProject()
{}

OmProject::~OmProject()
{}

QString OmProject::New(const QString& fnp)
{
    instance().impl_.reset(new OmProjectImpl());

    try{
        return instance().impl_->New(fnp);

    } catch(...){
        instance().impl_.reset();
        throw;
    }
}

void OmProject::Save(){
    instance().impl_->Save();
}

void OmProject::Commit(){
    instance().impl_->Commit();
}

void OmProject::Load(const QString& fileNameAndPath, QWidget* guiParent)
{
    instance().impl_.reset(new OmProjectImpl());

    try{
        instance().impl_->Load(fileNameAndPath, guiParent);

    } catch(...)
    {
        instance().impl_.reset();
        throw;
    }
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
#include "events/details/omEventManager.h"
#include "system/omOpenGLGarbageCollector.hpp"
#include "threads/omThreadPoolManager.h"
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

    OmTileCache::Delete();

    // OmProject must be deleted here, remaining singletons close cleanly
    instance().impl_.reset();

    //delete all singletons
    OmSegmentSelected::Delete();
    OmOpenGLGarbageCollector::Clear();
    OmPreferences::Delete();
    //OmLocalPreferences

    //close project data
    OmCacheManager::Delete();

    OmHdf5Manager::Delete();

    OmTilePools::Reset();
}
