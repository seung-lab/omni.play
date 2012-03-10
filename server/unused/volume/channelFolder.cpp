#include "volume/channelFolder.h"
#include "volume/channelImpl.h"
#include "datalayer/fs/fileNames.hpp"

namespace om {
namespace channel {

folder::folder(volume::channelImpl* vol)
    : vol_(vol)
{}

std::string folder::RelativeVolPath() const
{
    return str(boost::format("channels/channel%1%/")
               % vol_->GetID());
}

std::string folder::GetVolPath() const
{
    return str(boost::format("%1%/%2%")
        % datalayer::fileNames::FilesFolder()
        % RelativeVolPath());
}

std::string folder::MakeVolFolder() const
{
    static zi::rwmutex lock;

    const std::string fullPath = GetVolPath();

    datalayer::fileNames::CreateFolder(fullPath, lock);

    return fullPath;
}

}
}
