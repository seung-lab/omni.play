#pragma once

#include "common/debug.h"
#include "datalayer/fs/file.h"
#include "project/project.h"
#include "project/projectGlobals.h"

#include "utility/fileHelpers.h"
#include "utility/UUID.hpp"
#include "volume/segmentation.h"
#include "zi/omMutex.h"

namespace om {
namespace data {

class fileNames {
public:
    static inline void CreateFolder(const std::string& fullPath, zi::rwmutex& lock)
    {
        if(file::exists(fullPath)){
            return;
        }

        {
            zi::rwmutex::write_guard g(lock);

            if(!file::exists(fullPath)){
                utility::fileHelpers::MkDir(fullPath);
            }
        }
    }

    static std::string TempFileName(const utility::UUID& uuid){
        return file::tempPath() + "/omni.temp." + uuid.Str();
    }

    static std::string AddOmniExtensionIfNeeded(const std::string& str)
    {
        std::string fnp = str;
        if(fnp.rfind(".omni") == std::string::npos) {
            fnp += ".omni";
        }
        return fnp;
    }

    static void MakeFilesFolder()
    {
        static zi::rwmutex lock;
        CreateFolder(FilesFolder(), lock);
    }

    static std::string GetRandColorFileName() {
        return FilesFolder() + "/rand_colors.raw";
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
        const std::string subPath = vol->GetDirectoryPath() + "/" + level + "/";

        if(subPath.find("/") == 0){
            throw common::ioException("not a relative path: " + subPath.toStdString());
        }

        return FilesFolder() + "/" + subPath;
    }

    template <typename T>
    static std::string GetMemMapFileNameQT(T* vol, const int level)
    {
        static zi::rwmutex lock;
        zi::rwmutex::write_guard g(lock);

        const std::string fullPath = GetVolDataFolderPath(vol, level);

        if(!file::exists(fullPath)){
            if(!utility::fileHelpers::MkDir(fullPath))){
                throw common::ioException("could not create folder", fullPath);
            }
        }

        const std::string volType = vol->getVolDataTypeAsStr();

        const std::string fnp = "/" + fullPath + "/volume." + volType + ".raw");

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

} // namespace data
} // namespace om
