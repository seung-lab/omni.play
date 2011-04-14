#ifndef OM_MIP_MESH_MANAGERS_HPP
#define OM_MIP_MESH_MANAGERS_HPP

#include "mesh/omMeshManager.h"
#include "mesh/io/omMeshMetadata.hpp"
#include "utility/fuzzyStdObjs.hpp"
#include "utility/omStringHelpers.h"
#include "mesh/ziMesher.hpp"

class OmMeshManagers {
private:
    OmSegmentation *const segmentation_;

public:
    OmMeshManagers(OmSegmentation* segmentation)
        : segmentation_(segmentation)
    {}

    ~OmMeshManagers(){}

    void Load()
    {
        findMeshThresholds();

        // force manager load to start hdf5 conversion, if needed
        GetManager(1);
    }

    void CloseDownThreads()
    {
        FOR_EACH(iter, managers_){
            om::shared_ptr<OmMeshManager> man = iter->second;
            man->CloseDownThreads();
        }
    }

    bool AnyBuilt(){
        return GetManager(1)->Metadata()->IsBuilt();
    }

    OmMeshManager* GetManager(const double threshold)
    {
        if(!managers_.count(threshold)){
            managers_[threshold] =
                om::make_shared<OmMeshManager>(segmentation_, threshold);
            managers_[threshold]->Load();
        }

        return managers_[threshold].get();
    }

    void CreateManager(const double threshold)
    {
        managers_[threshold] =
            om::make_shared<OmMeshManager>(segmentation_, threshold);
        managers_[threshold]->Create();
    }

    const DoubleFuzzyStdSet& Thresholds() const {
        return thresholds_;
    }

    void FullMesh(const double threshold, const bool redownsample)
    {
        CreateManager(threshold);
        ziMesher mesher(segmentation_, threshold);
        mesher.MeshFullVolume(redownsample);
    }

/*
    void MeshChunk(const OmChunkCoord& coord)
    {
        ziMesher mesher(segmentation_, 1);
        mesher.addChunkCoord(coord);
        mesher.mesh();
    }

    void RebuildChunk(const OmChunkCoord&,
                      const OmSegIDsSet& )
    {
        assert(0);
//build chunk volume data and analyze data
//	OmMipVolume::BuildChunk(mipCoord);

//rebuild mesh data only if entire volume data has been built
if (IsVolumeDataBuilt()) {
}

//remove mesh from cache to force it to reload
foreach( const OmSegID & val, rModifiedValues ){
OmMeshCoord mip_mesh_coord = OmMeshCoord(mipCoord, val);
mMipMeshManager->UncacheMesh(mip_mesh_coord);
}

//call redraw to force mesh to reload
OmEvents::Redraw3d();
    }
*/

    void GetMesh(OmMeshPtr& ptr, const OmChunkCoord& coord,
                 const OmSegID segID, const double threshold,
                 const om::Blocking blocking = om::NON_BLOCKING)
    {
        return GetManager(threshold)->GetMesh(ptr, OmMeshCoord(coord, segID), blocking);
    }

private:
    DoubleFuzzyStdMap<om::shared_ptr<OmMeshManager> > managers_;
    DoubleFuzzyStdSet thresholds_;

    void findMeshThresholds()
    {
        const QString meshFolder =
            OmFileNames::GetMeshFolderPath(segmentation_);
        QDir dir(meshFolder);

        QStringList filters;
        filters << "?.????";
        dir.setNameFilters(filters);

        const QStringList dirNames = dir.entryList(QDir::Dirs);
        FOR_EACH(iter, dirNames){
            const QString& str = *iter;
            const double threshold = OmStringHelpers::getDouble(str);
            thresholds_.insert(threshold);
            std::cout << "found mesh threshold " << threshold << "\n";
        }
    }
};

#endif
