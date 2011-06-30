#pragma once

#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omSegmentationFolders.hpp"
#include "utility/omStringHelpers.h"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationFolder.h"

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

    void SwitchToUser(const std::string& userName){
        userFolder_ = QDir(usersFolderRoot_ + QString::fromStdString(userName)).absolutePath();
    }

    QString LogFolderPath(){
        return userFolder_ + QLatin1String("/logFiles");
    }

    void SetupFolders()
    {
        OmFileHelpers::MkDir(userFolder_);

        fixSegmentationFolderSymlinks();
    }

    inline std::string UsersFolder() const {
        return userFolder_.toStdString();
    }

    inline std::string UsersRootFolder() const {
        return usersFolderRoot_.toStdString();
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

        if(OmFileHelpers::IsSymlink(oldSegmentsFolder))
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
        const QDir filesDir(vol->Folder()->GetVolPath());
        return filesDir.absolutePath() + QLatin1String("/segments/");
    }
};

} // namespace om
