#include "volume/segmentationFolder.h"
#include "volume/segmentation.h"
#include "project/project.h"
#include "project/projectGlobals.h"

#include "datalayer/fs/fileNames.hpp"

om::segmentation::folder::folder(segmentation* vol)
    : vol_(vol)
{}

std::string om::segmentation::folder::GetVolSegmentsPathAbs()
{
    return str( boost::format("%1%/segmentations/segmentation%2%/segments")
                % project::Globals().Users().UsersFolder()
                % vol_->GetID());
}

std::string om::segmentation::folder::GetVolSegmentsPathAbs(const std::string& subFile)
{
    return str( boost::format("%1%/%2%")
                % GetVolSegmentsPathAbs()
                % subFile);
}

std::string om::segmentation::folder::RelativeVolPath()
{
    return std::string::fromStdString(str( boost::format("segmentations/segmentation%1%/")
                                       % vol_->GetID()));
}

std::string om::segmentation::folder::GetVolPath(){
    return fileNames::FilesFolder() + QLatin1String("/") + RelativeVolPath();
}

std::string om::segmentation::folder::GetMeshChunkFolderPath(const double threshold,
                                                         const coords::chunkCoord& coord)
{
    const QDir filesDir(GetVolPath());

    const std::string subPath =
        std::string("/meshes/%1/%2/%3/%4/%5/")
        .arg(std::string::number(threshold, 'f', 4))
        .arg(coord.getLevel())
        .arg(coord.X())
        .arg(coord.Y())
        .arg(coord.Z());

    return filesDir.absolutePath() + subPath;
}

std::string om::segmentation::folder::MakeMeshChunkFolderPath(const double threshold,
                                                          const coords::chunkCoord& coord)
{
    static zi::rwmutex lock;

    const std::string fullPath = GetMeshChunkFolderPath(threshold, coord);

    fileNames::CreateFolder(fullPath, lock);

    return fullPath;
}

std::string om::segmentation::folder::GetMeshFolderPath()
{
    const QDir filesDir(GetVolPath());
    return filesDir.absolutePath() + QLatin1String("/meshes/");
}

std::string om::segmentation::folder::MakeMeshFolderPath()
{
    static zi::rwmutex lock;

    const std::string fullPath = GetMeshFolderPath();
    fileNames::CreateFolder(fullPath, lock);
    return fullPath;
}

std::string om::segmentation::folder::GetMeshThresholdFolderPath(const double threshold)
{
    const QDir filesDir(GetVolPath());

    const std::string subPath =
        std::string("/meshes/%1/")
        .arg(std::string::number(threshold, 'f', 4));

    return filesDir.absolutePath() + subPath;
}

std::string om::segmentation::folder::MakeMeshThresholdFolderPath(const double threshold)
{
    static zi::rwmutex lock;

    const std::string fullPath = GetMeshThresholdFolderPath(threshold);
    fileNames::CreateFolder(fullPath, lock);
    return fullPath;
}

std::string om::segmentation::folder::MeshMetadataFileOld(){
    return MakeMeshFolderPath() + "meshMetadata.ver1";
}

std::string om::segmentation::folder::MeshMetadataFilePerThreshold(const double threshold){
    return MakeMeshThresholdFolderPath(threshold) + "meshMetadata.ver1";
}

std::string om::segmentation::folder::GetChunksFolder()
{
    const QDir filesDir(GetVolPath());
    return filesDir.absolutePath() + QLatin1String("/chunks/");
}

std::string om::segmentation::folder::GetChunkFolderPath(const coords::chunkCoord& coord)
{
    const QDir filesDir(GetVolPath());

    const std::string subPath =
        std::string("/chunks/%1/%2/%3/%4/")
        .arg(coord.getLevel())
        .arg(coord.X())
        .arg(coord.Y())
        .arg(coord.Z());

    return filesDir.absolutePath() + subPath;
}

std::string om::segmentation::folder::MakeChunkFolderPath(const coords::chunkCoord& coord)
{
    static zi::rwmutex lock;

    const std::string fullPath = GetChunkFolderPath(coord);

    fileNames::CreateFolder(fullPath, lock);

    return fullPath;
}

std::string om::segmentation::folder::MakeVolFolder()
{
    static zi::rwmutex lock;

    const std::string fullPath = GetVolPath();

    fileNames::CreateFolder(fullPath, lock);

    return fullPath;
}

std::string om::segmentation::folder::AnnotationFile()
{
    return std::string::fromStdString(
        str( boost::format("%1%/segmentations/segmentation%2%/annotations.yml")
             % project::Globals().Users().UsersFolder()
             % vol_->GetID())
        );
}

void om::segmentation::folder::MakeUserSegmentsFolder(){
    fileHelpers::MkDir(GetVolSegmentsPathAbs());
}

std::string om::segmentation::folder::LongRangeConnectionFile()
{
    return std::string::fromStdString(
        str( boost::format("%1%/segmentations/segmentation%2%/LongRangeConnections.txt")
             % project::Globals().Users().UsersFolder()
             % vol_->GetID())
        );
}
