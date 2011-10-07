#pragma once

#include "common/debug.h"
#include "datalayer/fs/file.h"
#include "project/project.h"
#include "project/projectGlobals.h"

#include "utility/omFileHelpers.h"
#include "utility/omUUID.hpp"
#include "volume/segmentation.h"
#include "zi/omMutex.h"

#include <QFile>
#include <QDir>

class fileNames {
public:
    static inline void CreateFolder(const std::string& fullPath, zi::rwmutex& lock)
    {
        if(QDir(fullPath).exists()){
            return;
        }

        {
            zi::rwmutex::write_guard g(lock);

            if(!QDir(fullPath).exists()){
                fileHelpers::MkDir(fullPath);
            }
        }
    }

    static std::string TempFileName(const OmUUID& uuid){
        return om::file::tempPath() + "/omni.temp." + uuid.Str();
    }

    static std::string AddOmniExtensionIfNeeded(const std::string& str)
    {
        if(NULL == str){
            return NULL;
        }

        std::string fnp = str;
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
    static std::string GetVolDataFolderPath(T* vol, const int level)
    {
        const std::string subPath = std::string("%1/%2/")
            .arg(std::string::fromStdString(vol->GetDirectoryPath()))
            .arg(level);

        if(subPath.startsWith("/")){
            throw OmIoException("not a relative path: " + subPath.toStdString());
        }

        return FilesFolder() + QLatin1String("/") + subPath;
    }

    template <typename T>
    static std::string GetMemMapFileNameQT(T* vol, const int level)
    {
        static zi::rwmutex lock;
        zi::rwmutex::write_guard g(lock);

        const std::string fullPath = GetVolDataFolderPath(vol, level);

        if(!QDir(fullPath).exists()){
            if(!QDir().mkpath(fullPath)){
                throw OmIoException("could not create folder", fullPath);
            }
        }

        const std::string volType = vol->getVolDataTypeAsStr();

        const std::string fnp = std::string("/%1/volume.%2.raw")
            .arg(fullPath)
            .arg(std::string::fromStdString(volType));

        const std::string fnp_clean = QDir::cleanPath(fnp);

        ZiLOG(DEBUG, io) << "file is " << fnp_clean.toStdString() << "\n";

        return fnp_clean;
    }

    static std::string LogFolderPath(){
        return project::Globals().Users().LogFolderPath();
    }

    static std::string ProjectMetadataFileOld(){
        return FilesFolder() + "/projectMetadata.qt";
    }

    static std::string ProjectMetadataFile(){
        return FilesFolder() + "/projectMetadata.yaml";
    }
    
    static std::string OldHDF5projectFileName(){
        return FilesFolder() + "/oldProjectFile.hdf5";
    }

    static std::string FilesFolder(){
        return project::FilesFolder();
    }
};

