#include "volume/channelFolder.h"
#include "volume/channelImpl.h"
#include "datalayer/fs/fileNames.hpp"

om::channel::folder::folder(channelImpl* vol)
    : vol_(vol)
{}

std::string om::channel::folder::RelativeVolPath() const
{
    return std::string::fromStdString(str( boost::format("channels/channel%1%/")
                                       % vol_->GetID()));
}

std::string om::channel::folder::GetVolPath() const
{
    return fileNames::FilesFolder()
        + QLatin1String("/")
        + RelativeVolPath();
}

std::string om::channel::folder::MakeVolFolder() const
{
    static zi::rwmutex lock;

    const std::string fullPath = GetVolPath();

    fileNames::CreateFolder(fullPath, lock);

    return fullPath;
}
