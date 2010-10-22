#ifndef OM_MIP_MESH_MANAGER_H
#define OM_MIP_MESH_MANAGER_H

#include "segment/omSegmentPointers.h"
#include "mesh/omMeshTypes.h"

class OmMipMeshCoord;
class OmMeshCache;
class OmMipMesh;
class OmSegmentation;

class OmMipMeshManager {
public:
	OmMipMeshManager(OmSegmentation* segmentation);
	~OmMipMeshManager();

	void CloseDownThreads();

	std::string GetDirectoryPath() const;

	//meshing
	OmMipMeshPtr AllocMesh(const OmMipMeshCoord&);
	void GetMesh(OmMipMeshPtr& p_value, const OmMipMeshCoord &coord );
	void UncacheMesh(const OmMipMeshCoord &coord );

private:
	OmSegmentation *const segmentation_;
	OmMeshCache *const mDataCache;

	void HandleFetchUpdate();

	friend QDataStream &operator<<(QDataStream&, const OmMipMeshManager&);
	friend QDataStream &operator>>(QDataStream&, OmMipMeshManager&);
};

#endif
