#pragma once

#include "mesh/io/meshMetadata.hpp"
#include "mesh/mesher/ziMesher.hpp"
#include "mesh/meshManager.h"
#include "utility/fuzzyStdObjs.hpp"
#include "utility/omStringHelpers.h"
#include "volume/segmentationFolder.h"

class meshManagers {
private:
    segmentation *const segmentation_;

public:
    meshManagers(segmentation* segmentation)
        : segmentation_(segmentation)
    {}

    ~meshManagers()
    {
        zi::guard g(managersLock_);

        FOR_EACH(iter, managers_){
            delete iter->second;
        }
    }

    void Load()
    {
        findMeshThresholds();

        // force manager load to start hdf5 conversion, if needed
        GetManager(1);
    }

    void CloseDownThreads()
    {
        zi::guard g(managersLock_);

        FOR_EACH(iter, managers_)
        {
            meshManager* man = iter->second;
            man->CloseDownThreads();
        }
    }

    bool AnyBuilt(){
        return GetManager(1)->Metadata()->IsBuilt();
    }

    meshManager* GetManager(const double threshold)
    {
        zi::guard g(managersLock_);

        if(!managers_.count(threshold))
        {
            managers_[threshold] = new meshManager(segmentation_, threshold);
            managers_[threshold]->Load();
        }

        return managers_[threshold];
    }

    void CreateManager(const double threshold)
    {
        zi::guard g(managersLock_);

        managers_[threshold] = new meshManager(segmentation_, threshold);
        managers_[threshold]->Create();
    }

    const DoubleFuzzyStdSet& Thresholds() const {
        return thresholds_;
    }

    void FullMesh(const double threshold)
    {
        CreateManager(threshold);
        ziMesher mesher(segmentation_, threshold);
        mesher.MeshFullVolume();
    }

    void FullMesh(const double threshold, om::shared_ptr<om::gui::progress> progress)
    {
        CreateManager(threshold);
        ziMesher mesher(segmentation_, threshold);
        mesher.Progress(progress);
        mesher.MeshFullVolume();
    }

    void ClearMeshCaches()
    {
        zi::guard g(managersLock_);

        FOR_EACH(iter, managers_)
        {
            meshManager* man = iter->second;
            man->ClearCache();
        }
    }

/*
    void MeshChunk(const coords::chunkCoord& coord)
    {
        ziMesher mesher(segmentation_, 1);
        mesher.addChunkCoord(coord);
        mesher.mesh();
    }

    void RebuildChunk(const coords::chunkCoord&,
                      const segIdsSet& )
    {
        assert(0);
//build chunk volume data and analyze data
//	mipVolume::BuildChunk(mipCoord);

//rebuild mesh data only if entire volume data has been built
if (IsVolumeDataBuilt()) {
}

//remove mesh from cache to force it to reload
foreach( const segId & val, rModifiedValues ){
meshCoord mip_mesh_coord = meshCoord(mipCoord, val);
mMipMeshManager->UncacheMesh(mip_mesh_coord);
}

//call redraw to force mesh to reload
OmEvents::Redraw3d();
    }
*/

    void GetMesh(meshPtr& ptr, const coords::chunkCoord& coord,
                 const segId segID, const double threshold,
                 const om::Blocking blocking = om::NON_BLOCKING)
    {
        return GetManager(threshold)->GetMesh(ptr, meshCoord(coord, segID), blocking);
    }

private:
    DoubleFuzzyStdMap<meshManager*> managers_;
    zi::spinlock managersLock_;

    DoubleFuzzyStdSet thresholds_;

    void findMeshThresholds()
    {
        const std::string meshFolder = segmentation_->Folder()->GetMeshFolderPath();
        QDir dir(meshFolder);

        std::stringList filters;
        filters << "?.????";
        dir.setNameFilters(filters);

        const std::stringList dirNames = dir.entryList(QDir::Dirs);

        FOR_EACH(iter, dirNames)
        {
            const std::string& str = *iter;
            const double threshold = OmStringHelpers::getDouble(str);
            thresholds_.insert(threshold);
            std::cout << "found mesh threshold " << threshold << "\n";
        }
    }
};

