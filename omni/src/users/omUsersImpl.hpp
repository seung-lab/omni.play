#pragma once

#include "utility/omStringHelpers.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omSegmentationFolders.hpp"
#include "volume/omSegmentation.h"

#include <QDir>

namespace om {

class usersImpl {
private:
    const QString usersFolderRoot_;
    QString userFolder_;

public:
    usersImpl()
        : usersFolderRoot_(usersFolderRoot())
    {}

    void SwitchToDefaultUser(){
        userFolder_ = QDir(usersFolderRoot_ + "_default").absolutePath();
    }

    QString GetVolSegmentsPathAbs(OmSegmentation* vol)
    {
        const QString subPath = QString::fromStdString(vol->GetDirectoryPath());
        return userFolder_ + QLatin1String("/") + subPath + QLatin1String("/segments/");
    }

    QString LogFolderPath(){
        return userFolder_ + QLatin1String("/logFiles");
    }

    void SetupFolders()
    {
        OmFileHelpers::MkDir(userFolder_);

        fixSegmentationFolderSymlinks();
    }

private:

    QString usersFolderRoot(){
        return OmFileNames::FilesFolder() + "/users/";
    }

    QString makeUserSegmentsFolder(const QString& folder)
    {
        QString dest = folder;

        dest.replace(OmFileNames::FilesFolder(), userFolder_);

        OmFileHelpers::MkDir(dest);

        return dest += "/segments";
    }

    void fixSegmentationFolderSymlinks()
    {
        const std::vector<om::fs::segmentationFolderInfo> segmentationFolders =
            om::fs::segmentationFolders::Find();

        FOR_EACH(iter, segmentationFolders)
        {
            const QString folder = iter->path;

            fixSegmentFolderSymlink(folder);
        }
    }

    void fixSegmentFolderSymlink(const QString& folder)
    {
        const QString oldSegmentsFolder = folder + "/segments";
        const QString userSegmentsFolder = makeUserSegmentsFolder(folder);

        if(OmFileHelpers::SymlinkExists(oldSegmentsFolder))
        {
            OmFileHelpers::RmFile(oldSegmentsFolder);
            OmFileHelpers::Symlink(userSegmentsFolder, oldSegmentsFolder);

        } else if(OmFileHelpers::IsFolder(oldSegmentsFolder))
        {
            OmFileHelpers::MoveFile(oldSegmentsFolder, userSegmentsFolder);
            OmFileHelpers::Symlink(userSegmentsFolder, oldSegmentsFolder);

        } else
        {
            // no symlink present
            OmFileHelpers::Symlink(userSegmentsFolder, oldSegmentsFolder);
        }
    }

    QString oldGetVolSegmentsPathAbs(OmSegmentation* vol)
    {
        const QDir filesDir(OmFileNames::GetVolPath(vol));
        return filesDir.absolutePath() + QLatin1String("/segments/");
    }
};

} // namespace om
