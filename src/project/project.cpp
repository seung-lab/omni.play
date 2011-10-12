#include "tiles/pools/tilePools.hpp"
#include "project/project.h"
#include "project/projectImpl.hpp"

namespace om {

project::project()
{}

project::~project()
{}

std::string project::New(const std::string& fnp)
{
    instance().impl_.reset(new projectImpl());

    try{
        return instance().impl_->New(fnp);

    } catch(...){
        instance().impl_.reset();
        throw;
    }
}

void project::Save(){
    instance().impl_->Save();
}

void project::Load(const std::string& fileNameAndPath, QWidget* guiParent)
{
    instance().impl_.reset(new projectImpl());

    try{
        instance().impl_->Load(fileNameAndPath, guiParent);

    } catch(...)
    {
        instance().impl_.reset();
        throw;
    }
}

const std::string& project::FilesFolder(){
    return instance().impl_->FilesFolder();
}

const std::string& project::OmniFile(){
    return instance().impl_->OmniFile();
}

bool project::HasOldHDF5(){
    return instance().impl_->HasOldHDF5();
}

OmHdf5* project::OldHDF5(){
    return instance().impl_->OldHDF5();
}

projectVolumes& project::Volumes(){
    return instance().impl_->Volumes();
}

int project::GetFileVersion(){
    return instance().impl_->GetFileVersion();
}

void project::setFileVersion(const int ver){
    instance().impl_->setFileVersion(ver);
}

bool project::IsReadOnly(){
    return instance().impl_->IsReadOnly();
}

bool project::IsOpen(){
    return instance().impl_;
}

#include "actions/omActions.h"
#include "segmentsegmentSelected.hpp"
#include "events/details/omEventManager.h"
#include "system/omOpenGLGarbageCollector.hpp"
#include "threads/omThreadPoolManager.h"
#include "zi/threads.h"

void project::Close()
{
    if(!IsOpen()){
        return;
    }

    OmActions::Close();

    OmCacheManager::SignalCachesToCloseDown();
    OmThreadPoolManager::StopAll();
    zi::all_threads::join_all();

    tileCache::Delete();

    // project must be deleted here, remaining singletons close cleanly
    instance().impl_.reset();

    //delete all singletons
    segmentSelected::Delete();
    OmOpenGLGarbageCollector::Clear();
    OmPreferences::Delete();
    //OmLocalPreferences

    //close project data
    OmCacheManager::Delete();

    OmHdf5Manager::Delete();

    tilePools::Reset();
}

}
