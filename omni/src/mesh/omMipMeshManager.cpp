#include "utility/omFileHelpers.h"
#include "common/omDebug.h"
#include "mesh/omMipMesh.h"
#include "mesh/omMipMeshManager.h"
#include "system/cache/omThreadedCache.h"
#include "chunks/omChunkCoord.h"
#include "volume/omSegmentation.h"
#include "system/cache/omMeshCache.h"
#include "mesh/io/v2/omMeshReaderV2.hpp"
#include "mesh/io/omMeshMetadata.hpp"
#include "mesh/io/omMeshConvertV1toV2.hpp"
#include "mesh/io/v2/omMeshFilePtrCache.hpp"

OmMipMeshManager::OmMipMeshManager(OmSegmentation* segmentation,
                                   const double threshold)
    : segmentation_(segmentation)
    , mDataCache(new OmMeshCache(this))
    , threshold_(threshold)
    , filePtrCache_(boost::make_shared<OmMeshFilePtrCache>(segmentation_, threshold))
    , metadata_(boost::make_shared<OmMeshMetadata>(segmentation_, threshold_))
{}

void OmMipMeshManager::Create()
{
    const QString path =
        OmFileNames::GetMeshThresholdFolderPath(segmentation_, threshold_);

    OmFileHelpers::RemoveDir(path);

    OmFileNames::MakeMeshThresholdFolderPath(segmentation_, threshold_);
}

void OmMipMeshManager::Load()
{
    if(qFuzzyCompare(1, threshold_)){
        loadThreadhold1();
    }else {
        loadThreadholdNon1();
    }

    reader_ = boost::make_shared<OmMeshReaderV2>(segmentation_, threshold_);
}

void OmMipMeshManager::loadThreadhold1()
{
    if(!metadata_->Load()){
        inferMeshMetadata();
    }

    if(metadata_->IsBuilt()){
        if(metadata_->IsHDF5()){
            ActivateConversionFromV1ToV2();
        }
    }
}

void OmMipMeshManager::loadThreadholdNon1()
{
    if(!metadata_->Load()){
        std::cout << "could not load mesh for " << threshold_ << "\n";
    }
}

void OmMipMeshManager::inferMeshMetadata()
{
    if(!OmProject::HasOldHDF5()){
        printf("no HDF5 file found\n");
        return;
    }

    OmMeshReaderV1 hdf5Reader(segmentation_);

    if(hdf5Reader.IsAnyMeshDataPresent()){
        metadata_->SetMeshedAndStorageAsHDF5();
        printf("HDF5 meshes found\n");
        return;
    }

    printf("no HDF5 meshes found\n");
}

OmMipMeshManager::~OmMipMeshManager(){
    delete mDataCache;
}

OmMipMeshPtr OmMipMeshManager::Produce(const OmMipMeshCoord& coord)
{
    return boost::make_shared<OmMipMesh>(segmentation_,
                                         coord,
                                         this,
                                         mDataCache);
}

void OmMipMeshManager::GetMesh(OmMipMeshPtr& ptr,
                               const OmMipMeshCoord& coord){
    mDataCache->Get(ptr, coord, om::NON_BLOCKING);
}

void OmMipMeshManager::UncacheMesh(const OmMipMeshCoord & coord){
    mDataCache->Remove(coord);
}

void OmMipMeshManager::CloseDownThreads(){
    mDataCache->closeDownThreads();
}

void OmMipMeshManager::ActivateConversionFromV1ToV2()
{
    converter_ =
        boost::make_shared<OmMeshConvertV1toV2>(segmentation_);
    converter_->Start();
}

