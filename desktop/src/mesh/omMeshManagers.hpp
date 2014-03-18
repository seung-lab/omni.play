#pragma once
#include "precomp.h"

#include "mesh/io/omMeshMetadata.hpp"
#include "mesh/mesher/ziMesher.hpp"
#include "mesh/omMeshManager.h"
#include "utility/fuzzyStdObjs.hpp"
#include "utility/omStringHelpers.h"
#include "volume/omSegmentationFolder.h"

class OmMeshManagers {
 private:
  OmSegmentation* const segmentation_;

 public:
  OmMeshManagers(OmSegmentation* segmentation) : segmentation_(segmentation) {}

  ~OmMeshManagers() {
    zi::guard g(managersLock_);

    FOR_EACH(iter, managers_) { delete iter->second; }
  }

  void Load() {
    findMeshThresholds();

    // force manager load to start hdf5 conversion, if needed
    GetManager(1);
  }

  void CloseDownThreads() {
    zi::guard g(managersLock_);

    FOR_EACH(iter, managers_) {
      OmMeshManager* man = iter->second;
      man->CloseDownThreads();
    }
  }

  bool AnyBuilt() { return GetManager(1)->Metadata()->IsBuilt(); }

  OmMeshManager* GetManager(const double threshold) {
    zi::guard g(managersLock_);

    if (!managers_.count(threshold)) {
      managers_[threshold] = new OmMeshManager(segmentation_, threshold);
      managers_[threshold]->Load();
    }

    return managers_[threshold];
  }

  void CreateManager(const double threshold) {
    zi::guard g(managersLock_);

    managers_[threshold] = new OmMeshManager(segmentation_, threshold);
    managers_[threshold]->Create();
  }

  const DoubleFuzzyStdSet& Thresholds() const { return thresholds_; }

  void FullMesh(const double threshold) {
    CreateManager(threshold);
    ziMesher mesher(segmentation_, threshold);
    mesher.MeshFullVolume();
  }

  void FullMesh(const double threshold,
                std::shared_ptr<om::gui::progress> progress) {
    CreateManager(threshold);
    ziMesher mesher(segmentation_, threshold);
    mesher.Progress(progress);
    mesher.MeshFullVolume();
  }

  void ClearMeshCaches() {
    zi::guard g(managersLock_);

    FOR_EACH(iter, managers_) {
      OmMeshManager* man = iter->second;
      man->ClearCache();
    }
  }

  /*
      void MeshChunk(const om::coords::Chunk& coord)
      {
          ziMesher mesher(segmentation_, 1);
          mesher.addChunkCoord(coord);
          mesher.mesh();
      }

      void RebuildChunk(const om::coords::Chunk&,
                        const om::common::SegIDSet& )
      {
          assert(0);
  //build chunk volume data and analyze data
  //  OmMipVolume::BuildChunk(mipCoord);

  //rebuild mesh data only if entire volume data has been built
  if (IsVolumeDataBuilt()) {
  }

  //remove mesh from cache to force it to reload
  foreach( const om::common::SegID & val, rModifiedValues ){
  om::coords::Mesh mip_mesh_coord = om::coords::Mesh(mipCoord, val);
  mMipMeshManager->UncacheMesh(mip_mesh_coord);
  }

  //call redraw to force mesh to reload
  om::event::Redraw3d();
      }
  */

  void GetMesh(OmMeshPtr& ptr, const om::coords::Chunk& coord,
               const om::common::SegID segID, const double threshold,
               const om::common::Blocking blocking =
                   om::common::Blocking::NON_BLOCKING) {
    return GetManager(threshold)
        ->GetMesh(ptr, om::coords::Mesh(coord, segID), blocking);
  }

 private:
  DoubleFuzzyStdMap<OmMeshManager*> managers_;
  zi::spinlock managersLock_;

  DoubleFuzzyStdSet thresholds_;

  void findMeshThresholds() {
    const QString meshFolder = segmentation_->Folder().GetMeshFolderPath();
    QDir dir(meshFolder);

    QStringList filters;
    filters << "?.????";
    dir.setNameFilters(filters);

    const QStringList dirNames = dir.entryList(QDir::Dirs);

    FOR_EACH(iter, dirNames) {
      const QString& str = *iter;
      const double threshold = OmStringHelpers::getDouble(str);
      thresholds_.insert(threshold);
      log_infos << "found mesh threshold " << threshold;
    }
  }
};
