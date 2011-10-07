#include "common/debug.h"
#include "mesh/io/meshConvertV1toV2.hpp"
#include "mesh/io/meshMetadata.hpp"
#include "mesh/iomeshFilePtrCache.hpp"
#include "mesh/iomeshReader.hpp"
#include "mesh/mesh.h"
#include "mesh/meshManager.h"
#include "system/cache/meshCache.h"
#include "utility/omFileHelpers.h"
#include "volume/segmentation.h"

meshManager::meshManager(segmentation* segmentation,
                             const double threshold)
    : segmentation_(segmentation)
    , threshold_(threshold)
    , dataCache_(new meshCache(this))
    , filePtrCache_(new meshFilePtrCache(segmentation_, threshold))
    , metadata_(new meshMetadata(segmentation_, threshold_))
{}

meshManager::~meshManager()
{}

void meshManager::Create()
{
    const std::string path = segmentation_->Folder()->GetMeshThresholdFolderPath(threshold_);

    fileHelpers::RemoveDir(path);

    segmentation_->Folder()->MakeMeshThresholdFolderPath(threshold_);
}

void meshManager::Load()
{
    if(qFuzzyCompare(1, threshold_)){
        loadThreadhold1();

    }else {
        loadThreadholdNon1();
    }

    reader_.reset(new meshReader(this));
}

void meshManager::loadThreadhold1()
{
    if(!metadata_->Load()){
        inferMeshMetadata();
    }

    if(metadata_->IsBuilt())
    {
        if(metadata_->IsHDF5())
        {
            if(project::HasOldHDF5()){
                ActivateConversionFromV1ToV2();
            }
            // TODO: else? mesh conversion probably wasn't finished...
        }
    }
}

void meshManager::loadThreadholdNon1()
{
    if(!metadata_->Load()){
        std::cout << "could not load mesh for " << threshold_ << "\n";
    }
}

void meshManager::inferMeshMetadata()
{
    if(!project::HasOldHDF5())
    {
        printf("no HDF5 file found\n");
        return;
    }

    meshReaderV1 hdf5Reader(segmentation_);

    if(hdf5Reader.IsAnyMeshDataPresent())
    {
        metadata_->SetMeshedAndStorageAsHDF5();
        printf("HDF5 meshes found\n");
        return;
    }

    printf("no HDF5 meshes found\n");
}

meshPtr meshManager::Produce(const meshCoord& coord)
{
    return om::make_shared<mesh>(segmentation_,
                                   coord,
                                   this,
                                   dataCache_.get());
}

void meshManager::GetMesh(meshPtr& ptr, const meshCoord& coord,
                            const om::Blocking blocking)
{
    dataCache_->Get(ptr, coord, blocking);
}

void meshManager::UncacheMesh(const meshCoord & coord){
    dataCache_->Remove(coord);
}

void meshManager::CloseDownThreads(){
    dataCache_->CloseDownThreads();
}

void meshManager::ActivateConversionFromV1ToV2()
{
    converter_.reset(new meshConvertV1toV2(this));
    converter_->Start();
}

void meshManager::ClearCache(){
    dataCache_->Clear();
}
