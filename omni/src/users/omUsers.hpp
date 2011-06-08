#pragma once

#include "datalayer/fs/omFileNames.hpp"

namespace om {

class users {
private:
    const QString usersFolderRoot_;
    QString userFolder_;

    QString usersFolderRoot(){
        return OmFileNames::FilesFolder() + "/users/";
    }

public:
    users()
        : usersFolderRoot_(usersFolderRoot())
    {
        SwitchToDefaultUser();
    }

    void SwitchToDefaultUser(){
        userFolder_ = QDir(usersFolderRoot_ + "_default").absolutePath();
    }

    // QString GetVolSegmentsPathAbs(OmSegmentation* vol)
    // {
    //     const QDir filesDir(GetVolPath(vol));
    //     return userFolder_ + QLatin1String("/segments/");
    // }

    void SetupFolders()
    {
        if(QDir(usersFolderRoot_).exists()){
            return;
        }

        upgrade();
    }

private:
    void upgrade()
    {
        OmFileHelpers::MkDir(usersFolderRoot_);

    }
};

} // namespace om
