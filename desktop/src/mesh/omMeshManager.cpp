#include "common/logging.h"
#include "mesh/io/omMeshConvertV1toV2.hpp"
#include "mesh/io/omMeshMetadata.hpp"
#include "mesh/io/v2/omMeshFilePtrCache.hpp"
#include "mesh/io/v2/omMeshReaderV2.hpp"
#include "mesh/omMesh.h"
#include "mesh/omMeshManager.h"
#include "system/cache/omMeshCache.h"
#include "utility/omFileHelpers.h"
#include "volume/omSegmentation.h"

OmMeshManager::OmMeshManager(OmSegmentation* segmentation,
                             const double threshold)
    : segmentation_(segmentation),
      threshold_(threshold),
      dataCache_(new OmMeshCache(this)),
      filePtrCache_(new OmMeshFilePtrCache(segmentation_, threshold)),
      metadata_(new OmMeshMetadata(segmentation_, threshold_)) {}

OmMeshManager::~OmMeshManager() {}

void OmMeshManager::Create() {
  const QString path =
      segmentation_->Folder()->GetMeshThresholdFolderPath(threshold_);

  OmFileHelpers::RemoveDir(path);

  segmentation_->Folder()->MakeMeshThresholdFolderPath(threshold_);

  reader_.reset(new OmMeshReaderV2(this));
}

void OmMeshManager::Load() {
  if (qFuzzyCompare(1, threshold_)) {
    loadThreadhold1();

  } else {
    loadThreadholdNon1();
  }

  reader_.reset(new OmMeshReaderV2(this));
}

void OmMeshManager::loadThreadhold1() {
  if (!metadata_->Load()) {
    inferMeshMetadata();
  }

  if (metadata_->IsBuilt()) {
    if (metadata_->IsHDF5()) {
      if (OmProject::HasOldHDF5()) {
        ActivateConversionFromV1ToV2();
      }
      // TODO: else? mesh conversion probably wasn't finished...
    }
  }
}

void OmMeshManager::loadThreadholdNon1() {
  if (!metadata_->Load()) {
    log_infos << "could not load mesh for " << threshold_;
  }
}

void OmMeshManager::inferMeshMetadata() {
  if (!OmProject::HasOldHDF5()) {
    log_infos << "no HDF5 file found";
    return;
  }

  OmMeshReaderV1 hdf5Reader(segmentation_);

  if (hdf5Reader.IsAnyMeshDataPresent()) {
    metadata_->SetMeshedAndStorageAsHDF5();
    log_infos << "HDF5 meshes found";
    return;
  }

  log_infos << "no HDF5 meshes found";
}

OmMeshPtr OmMeshManager::Produce(const OmMeshCoord& coord) {
  return std::make_shared<OmMesh>(segmentation_, coord, this, dataCache_.get());
}

void OmMeshManager::GetMesh(OmMeshPtr& ptr, const OmMeshCoord& coord,
                            const om::common::Blocking blocking) {
  dataCache_->Get(ptr, coord, blocking);
}

void OmMeshManager::UncacheMesh(const OmMeshCoord& coord) {
  dataCache_->Remove(coord);
}

void OmMeshManager::CloseDownThreads() { dataCache_->CloseDownThreads(); }

void OmMeshManager::ActivateConversionFromV1ToV2() {
  converter_.reset(new OmMeshConvertV1toV2(this));
  converter_->Start();
}

void OmMeshManager::ClearCache() { dataCache_->Clear(); }
