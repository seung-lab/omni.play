#include "volume/omSegmentationFolder.h"
#include "volume/omSegmentation.h"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "users/omUsers.h"
#include "datalayer/fs/omFileNames.hpp"

om::segmentation::folder::folder(OmSegmentation* vol)
    : vol_(vol)
{}

std::string om::segmentation::folder::GetVolSegmentsPathAbs()
{
    return str( boost::format("%1%/segmentations/segmentation%2%/segments")
                % OmProject::Globals().Users().UsersFolder()
                % vol_->GetID());
}

std::string om::segmentation::folder::GetVolSegmentsPathAbs(const std::string& subFile)
{
    return str( boost::format("%1%/%2%")
                % GetVolSegmentsPathAbs()
                % subFile);
}

QString om::segmentation::folder::RelativeVolPath()
{
    return QString::fromStdString(str( boost::format("segmentations/segmentation%1%/")
                                       % vol_->GetID()));
}

QString om::segmentation::folder::GetVolPath(){
    return OmFileNames::FilesFolder() + QLatin1String("/") + RelativeVolPath();
}

QString om::segmentation::folder::GetMeshChunkFolderPath(const double threshold,
                                                         const om::coords::Chunk& coord)
{
    const QDir filesDir(GetVolPath());

    const QString subPath =
        QString("/meshes/%1/%2/%3/%4/%5/")
        .arg(QString::number(threshold, 'f', 4))
        .arg(coord.mipLevel())
        .arg(coord.x)
        .arg(coord.y)
        .arg(coord.z);

    return filesDir.absolutePath() + subPath;
}

QString om::segmentation::folder::MakeMeshChunkFolderPath(const double threshold,
                                                          const om::coords::Chunk& coord)
{
    static zi::rwmutex lock;

    const QString fullPath = GetMeshChunkFolderPath(threshold, coord);

    OmFileNames::CreateFolder(fullPath, lock);

    return fullPath;
}

QString om::segmentation::folder::GetMeshFolderPath()
{
    const QDir filesDir(GetVolPath());
    return filesDir.absolutePath() + QLatin1String("/meshes/");
}

QString om::segmentation::folder::MakeMeshFolderPath()
{
    static zi::rwmutex lock;

    const QString fullPath = GetMeshFolderPath();
    OmFileNames::CreateFolder(fullPath, lock);
    return fullPath;
}

QString om::segmentation::folder::GetMeshThresholdFolderPath(const double threshold)
{
    const QDir filesDir(GetVolPath());

    const QString subPath =
        QString("/meshes/%1/")
        .arg(QString::number(threshold, 'f', 4));

    return filesDir.absolutePath() + subPath;
}

QString om::segmentation::folder::MakeMeshThresholdFolderPath(const double threshold)
{
    static zi::rwmutex lock;

    const QString fullPath = GetMeshThresholdFolderPath(threshold);
    OmFileNames::CreateFolder(fullPath, lock);
    return fullPath;
}

QString om::segmentation::folder::MeshMetadataFileOld(){
    return MakeMeshFolderPath() + "meshMetadata.ver1";
}

QString om::segmentation::folder::MeshMetadataFilePerThreshold(const double threshold){
    return MakeMeshThresholdFolderPath(threshold) + "meshMetadata.ver1";
}

QString om::segmentation::folder::GetChunksFolder()
{
    const QDir filesDir(GetVolPath());
    return filesDir.absolutePath() + QLatin1String("/chunks/");
}

QString om::segmentation::folder::GetChunkFolderPath(const om::coords::Chunk& coord)
{
    const QDir filesDir(GetVolPath());

    const QString subPath =
        QString("/chunks/%1/%2/%3/%4/")
        .arg(coord.mipLevel())
        .arg(coord.x)
        .arg(coord.y)
        .arg(coord.z);

    return filesDir.absolutePath() + subPath;
}

QString om::segmentation::folder::MakeChunkFolderPath(const om::coords::Chunk& coord)
{
    static zi::rwmutex lock;

    const QString fullPath = GetChunkFolderPath(coord);

    OmFileNames::CreateFolder(fullPath, lock);

    return fullPath;
}

QString om::segmentation::folder::MakeVolFolder()
{
    static zi::rwmutex lock;

    const QString fullPath = GetVolPath();

    OmFileNames::CreateFolder(fullPath, lock);

    return fullPath;
}

QString om::segmentation::folder::AnnotationFile()
{
    return QString::fromStdString(
        str( boost::format("%1%/segmentations/segmentation%2%/annotations.yml")
             % OmProject::Globals().Users().UsersFolder()
             % vol_->GetID())
        );
}

void om::segmentation::folder::MakeUserSegmentsFolder(){
    OmFileHelpers::MkDir(GetVolSegmentsPathAbs());
}

QString om::segmentation::folder::LongRangeConnectionFile()
{
    return QString::fromStdString(
        str( boost::format("%1%/segmentations/segmentation%2%/LongRangeConnections.txt")
             % OmProject::Globals().Users().UsersFolder()
             % vol_->GetID())
        );
}
