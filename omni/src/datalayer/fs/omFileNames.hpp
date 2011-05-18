#pragma once

#include "chunks/omChunkCoord.h"
#include "common/omDebug.h"
#include "datalayer/fs/omFile.hpp"
#include "project/omProject.h"
#include "utility/omFileHelpers.h"
#include "utility/omUUID.hpp"
#include "volume/omSegmentation.h"
#include "zi/omMutex.h"

#include <QFile>
#include <QDir>

class OmFileNames {
private:
    static inline void createFolder(const QString& fullPath, zi::rwmutex& lock)
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

public:

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
        createFolder(OmProject::FilesFolder(), lock);
    }

    static std::string GetRandColorFileName() {
        return OmProject::FilesFolder().toStdString() + "/rand_colors.raw";
    }

    template <typename T>
    static QString GetVolPath(T const*const vol)
    {
        const QString subPath = QString::fromStdString(vol->GetDirectoryPath());

        if(subPath.startsWith("/")){
            throw OmIoException("not a relative path", subPath);
        }

        return OmProject::FilesFolder() + QLatin1String("/") + subPath;
    }

    template <typename T>
    static QString MakeVolFolder(T* vol)
    {
        static zi::rwmutex lock;

        const QString fullPath = GetVolPath(vol);
        createFolder(fullPath, lock);

        return fullPath;
    }

    static QString GetVolSegmentsPath(OmSegmentation* vol)
    {
        const QDir filesDir(GetVolPath(vol));

        const QString subPath("segments/");

        return filesDir.absolutePath() + QLatin1String("/") + subPath;
    }

    static QString MakeVolSegmentsPath(OmSegmentation* vol)
    {
        static zi::rwmutex lock;

        const QString fullPath = GetVolSegmentsPath(vol);
        createFolder(fullPath, lock);
        return fullPath;
    }

    static QString GetMeshChunkFolderPath(OmSegmentation* vol,
                                          const double threshold,
                                          const OmChunkCoord& coord)
    {
        const QDir filesDir(GetVolPath(vol));

        const QString subPath =
            QString("meshes/%1/%2/%3/%4/%5/")
            .arg(QString::number(threshold, 'f', 4))
            .arg(coord.getLevel())
            .arg(coord.getCoordinateX())
            .arg(coord.getCoordinateY())
            .arg(coord.getCoordinateZ());

        return filesDir.absolutePath() + QLatin1String("/") + subPath;
    }

    static QString MakeMeshChunkFolderPath(OmSegmentation* vol,
                                           const double threshold,
                                           const OmChunkCoord& coord)
    {
        static zi::rwmutex lock;

        const QString fullPath = GetMeshChunkFolderPath(vol,
                                                        threshold,
                                                        coord);
        createFolder(fullPath, lock);
        return fullPath;
    }

    static QString GetMeshFolderPath(OmSegmentation* vol)
    {
        const QDir filesDir(GetVolPath(vol));

        const QString subPath("meshes/");

        return filesDir.absolutePath() + QLatin1String("/") + subPath;
    }

    static QString MakeMeshFolderPath(OmSegmentation* vol)
    {
        static zi::rwmutex lock;

        const QString fullPath = GetMeshFolderPath(vol);
        createFolder(fullPath, lock);
        return fullPath;
    }

    static QString GetMeshThresholdFolderPath(OmSegmentation* vol,
                                              const double threshold)
    {
        const QDir filesDir(GetVolPath(vol));

        const QString subPath =
            QString("meshes/%1/")
            .arg(QString::number(threshold, 'f', 4));

        return filesDir.absolutePath() + QLatin1String("/") + subPath;
    }

    static QString MakeMeshThresholdFolderPath(OmSegmentation* vol,
                                               const double threshold)
    {
        static zi::rwmutex lock;

        const QString fullPath = GetMeshThresholdFolderPath(vol, threshold);
        createFolder(fullPath, lock);
        return fullPath;
    }

    static QString MeshMetadataFileOld(OmSegmentation* segmentation)
    {
        const QString volPath =
            OmFileNames::MakeMeshFolderPath(segmentation);
        const QString fullPath = QString("%1meshMetadata.ver1")
            .arg(volPath);

        return fullPath;
    }

    static QString MeshMetadataFilePerThreshold(OmSegmentation* segmentation,
                                                const double threshold)
    {
        const QString volPath =
            OmFileNames::MakeMeshThresholdFolderPath(segmentation,
                                                     threshold);
        const QString fullPath = QString("%1meshMetadata.ver1")
            .arg(volPath);

        return fullPath;
    }

    //TODO: cleanup!
    //ex:  /home/projectName.files/segmentations/segmentation1/0/volume.int32_t.raw
    template <typename T>
    static std::string GetMemMapFileName(T* vol, const int level)
    {
        return GetMemMapFileNameQT(vol, level).toStdString();
    }

    template <typename T>
    static QString GetVolDataFolderPath(T* vol, const int level)
    {
        const QString subPath = QString("%1/%2/")
            .arg(QString::fromStdString(vol->GetDirectoryPath()))
            .arg(level);

        if(subPath.startsWith("/")){
            throw OmIoException("not a relative path: " + subPath.toStdString());
        }

        return OmProject::FilesFolder() + QLatin1String("/") + subPath;
    }

    template <typename T>
    static QString GetMemMapFileNameQT(T* vol, const int level)
    {
        static zi::rwmutex lock;
        zi::rwmutex::write_guard g(lock);

        const QString fullPath = GetVolDataFolderPath(vol, level);

        if(!QDir(fullPath).exists()){
            if(!QDir().mkpath(fullPath)){
                throw OmIoException("could not create folder", fullPath);
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

    static QString GetChunksFolder(OmSegmentation* vol)
    {
        const QDir filesDir(GetVolPath(vol));
        const QString subPath =	QString("chunks/");
        return filesDir.absolutePath() + QLatin1String("/") + subPath;
    }

    static QString GetChunkFolderPath(OmSegmentation* vol,
                                      const OmChunkCoord& coord)
    {
        const QDir filesDir(GetVolPath(vol));

        const QString subPath =
            QString("chunks/%1/%2/%3/%4/")
            .arg(coord.getLevel())
            .arg(coord.getCoordinateX())
            .arg(coord.getCoordinateY())
            .arg(coord.getCoordinateZ());

        return filesDir.absolutePath() + QLatin1String("/") + subPath;
    }

    static QString MakeChunkFolderPath(OmSegmentation* vol,
                                       const OmChunkCoord& coord)
    {
        static zi::rwmutex lock;

        const QString fullPath = GetChunkFolderPath(vol, coord);
        createFolder(fullPath, lock);
        return fullPath;
    }
};

