#pragma once

#include "common/omDebug.h"
#include "datalayer/fs/omFile.hpp"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "users/omUsers.h"
#include "utility/omFileHelpers.h"
#include "utility/omUUID.hpp"
#include "volume/omSegmentation.h"
#include "zi/omMutex.h"

#include <QFile>
#include <QDir>

class OmFileNames {
public:
    static inline void CreateFolder(const QString& fullPath, zi::rwmutex& lock)
    {
        if(QDir(fullPath).exists()){
            return;
        }

        {
            zi::rwmutex::write_guard g(lock);

            if(!QDir(fullPath).exists()){
                OmFileHelpers::MkDir(fullPath);
            }
        }
    }

    static std::string TempFileName(const OmUUID& uuid){
        return om::file::tempPath() + "/omni.temp." + uuid.Str();
    }

    static QString AddOmniExtensionIfNeeded(const QString& str)
    {
        if(NULL == str){
            return NULL;
        }

        QString fnp = str;
        if(!fnp.endsWith(".omni")) {
            fnp.append(".omni");
        }
        return fnp;
    }

    static void MakeFilesFolder()
    {
        static zi::rwmutex lock;
        CreateFolder(FilesFolder(), lock);
    }

    static std::string GetRandColorFileName() {
        return FilesFolder().toStdString() + "/rand_colors.raw";
    }

    //TODO: cleanup!
    //ex:  /home/projectName.files/segmentations/segmentation1/0/volume.int32_t.raw
    template <typename T>
    static std::string GetMemMapFileName(T* vol, const int level){
        return GetMemMapFileNameQT(vol, level).toStdString();
    }

    template <typename T>
    static QString GetVolDataFolderPath(T* vol, const int level)
    {
        const QString subPath = QString("%1/%2/")
            .arg(QString::fromStdString(vol->GetDirectoryPath()))
            .arg(level);

        if(subPath.startsWith("/")){
            throw IoException("not a relative path: " + subPath.toStdString());
        }

        return FilesFolder() + QLatin1String("/") + subPath;
    }

    template <typename T>
    static QString GetMemMapFileNameQT(T* vol, const int level)
    {
        static zi::rwmutex lock;
        zi::rwmutex::write_guard g(lock);

        const QString fullPath = GetVolDataFolderPath(vol, level);

        if(!QDir(fullPath).exists()){
            if(!QDir().mkpath(fullPath)){
                throw IoException("could not create folder", fullPath);
            }
        }

        const std::string volType = vol->getVolDataTypeAsStr();

        const QString fnp = QString("/%1/volume.%2.raw")
            .arg(fullPath)
            .arg(QString::fromStdString(volType));

        const QString fnp_clean = QDir::cleanPath(fnp);

        ZiLOG(DEBUG, io) << "file is " << fnp_clean.toStdString() << "\n";

        return fnp_clean;
    }

    static QString LogFolderPath(){
        return OmProject::Globals().Users().LogFolderPath();
    }

    static QString ProjectMetadataFileOld(){
        return FilesFolder() + "/projectMetadata.qt";
    }

    static QString ProjectMetadataFile(){
        return FilesFolder() + "/projectMetadata.yaml";
    }
    
    static QString OldHDF5projectFileName(){
        return FilesFolder() + "/oldProjectFile.hdf5";
    }

    static QString FilesFolder(){
        return OmProject::FilesFolder();
    }
};

