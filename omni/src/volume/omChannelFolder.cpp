#include "volume/omChannelFolder.h"
#include "volume/omChannelImpl.h"
#include "datalayer/fs/omFileNames.hpp"

om::channel::folder::folder(OmChannelImpl* vol)
    : vol_(vol)
{}

QString om::channel::folder::RelativeVolPath() const
{
    return QString::fromStdString(str( boost::format("channels/channel%1%/")
                                       % vol_->GetID()));
}

QString om::channel::folder::GetVolPath() const
{
    return OmFileNames::FilesFolder()
        + QLatin1String("/")
        + RelativeVolPath();
}

QString om::channel::folder::MakeVolFolder() const
{
    static zi::rwmutex lock;

    const QString fullPath = GetVolPath();

    OmFileNames::CreateFolder(fullPath, lock);

    return fullPath;
}
