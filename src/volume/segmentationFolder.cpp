#include "volume/segmentationFolder.h"
#include "volume/segmentation.h"
#include "project/project.h"

#include "datalayer/fs/fileNames.hpp"

namespace om {
namespace segmentation {

folder::folder(volume::segmentation* vol)
    : vol_(vol)
{}

std::string folder::GetVolSegmentsPathAbs()
{
    return str( boost::format("%1%/users/_default/segmentations/segmentation%2%/segments")
                % file::absolute(GetVolPath())
                % vol_->GetID());
}

std::string folder::GetVolSegmentsPathAbs(const std::string& subFile)
{
    return str( boost::format("%1%/%2%")
                % GetVolSegmentsPathAbs()
                % subFile);
}

std::string folder::RelativeVolPath()
{
    return str( boost::format("segmentations/segmentation%1%/")
                                       % vol_->GetID());
}

std::string folder::GetVolPath(){
    return datalayer::fileNames::FilesFolder() + "/" + RelativeVolPath();
}

std::string folder::GetMeshChunkFolderPath(const double threshold,
                                           const coords::chunkCoord& coord)
{
    return str(boost::format("%1%/%2%/%3%/%4%/%5%")
               % GetMeshThresholdFolderPath(threshold)
               % (coord.getLevel())
               % (coord.X())
               % (coord.Y())
               % (coord.Z()));
}

std::string folder::MakeMeshChunkFolderPath(const double threshold,
                                            const coords::chunkCoord& coord)
{
    static zi::rwmutex lock;

    const std::string fullPath = GetMeshChunkFolderPath(threshold, coord);

    datalayer::fileNames::CreateFolder(fullPath, lock);

    return fullPath;
}

std::string folder::GetMeshFolderPath()
{
    return file::absolute(GetVolPath()) + "/meshes/";
}

std::string folder::MakeMeshFolderPath()
{
    static zi::rwmutex lock;

    const std::string fullPath = GetMeshFolderPath();
    datalayer::fileNames::CreateFolder(fullPath, lock);
    return fullPath;
}

std::string folder::GetMeshThresholdFolderPath(const double threshold)
{
    return str(boost::format("%1%/meshes/%2$0.4f/")
               % file::absolute(GetVolPath())
               % threshold);
}

std::string folder::MakeMeshThresholdFolderPath(const double threshold)
{
    static zi::rwmutex lock;

    const std::string fullPath = GetMeshThresholdFolderPath(threshold);
    datalayer::fileNames::CreateFolder(fullPath, lock);
    return fullPath;
}

std::string folder::MeshMetadataFileOld(){
    return MakeMeshFolderPath() + "meshMetadata.ver1";
}

std::string folder::MeshMetadataFilePerThreshold(const double threshold){
    return MakeMeshThresholdFolderPath(threshold) + "meshMetadata.ver1";
}

std::string folder::GetChunksFolder()
{
    return file::absolute(GetVolPath()) + "/chunks/";
}

std::string folder::GetChunkFolderPath(const coords::chunkCoord& coord)
{
    return str(boost::format("%1%/chunks/%2%/%3%/%4%/%5%")
               % file::absolute(GetVolPath())
               % coord.getLevel()
               % coord.X()
               % coord.Y()
               % coord.Z());
}

std::string folder::MakeChunkFolderPath(const coords::chunkCoord& coord)
{
    static zi::rwmutex lock;

    const std::string fullPath = GetChunkFolderPath(coord);

    datalayer::fileNames::CreateFolder(fullPath, lock);

    return fullPath;
}

std::string folder::MakeVolFolder()
{
    static zi::rwmutex lock;

    const std::string fullPath = GetVolPath();

    datalayer::fileNames::CreateFolder(fullPath, lock);

    return fullPath;
}

void folder::MakeUserSegmentsFolder(){
    utility::fileHelpers::MkDir(GetVolSegmentsPathAbs());
}

} // namespace segmentation
} // namespace om
