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

zi::semaphore& FileReadSemaphore(){
    return instance().impl_->FileReadSemaphore();
}


#include "threads/threadPoolManager.h"
#include "zi/threads.h"

void project::Close()
{
    if(!IsOpen()){
        return;
    }

    threadPoolManager::StopAll();
    zi::all_threads::join_all();

    // project must be deleted here, remaining singletons close cleanly
    instance().impl_.reset();
}

}
