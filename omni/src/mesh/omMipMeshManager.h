#ifndef OM_MIP_MESH_MANAGER_H
#define OM_MIP_MESH_MANAGER_H

/*
 *
 *
 */


#include "omMipMesh.h"
#include "omMipMeshCoord.h"

#include "common/omStd.h"
#include "system/omThreadedCache.h"

typedef OmThreadedCache< OmMipMeshCoord, OmMipMesh > MipMeshCache;


class OmMipChunk;
class OmMipChunkCoord;
class OmMeshSource;
class OmSegmentManager;
class QGLContext;


class OmMipMeshManager : public MipMeshCache {

public:
	OmMipMeshManager();
	~OmMipMeshManager();
	
	//accessors
	const string& GetDirectoryPath() const;
	void SetDirectoryPath(const string &);
	
	bool IsMeshDataBuilt();
	void SetMeshDataBuilt(bool);
	
	
	//meshing
	OmMipMesh* AllocMesh(const OmMipMeshCoord &coord );
	void GetMesh(shared_ptr<OmMipMesh> &p_value, const OmMipMeshCoord &coord );
	void UncacheMesh(const OmMipMeshCoord &coord );
	
	//building
	void BuildChunkMeshes(OmMipChunk& chunk, const SegmentDataSet &);

	
	//drawing
	void DrawMeshes(OmSegmentManager &rSegMgr,
					const OmBitfield &drawOps,
					const OmMipChunkCoord &mipCoord,
					const SegmentDataSet &rRelvDataVals );
	
	
	
private:
	OmMipMesh* HandleCacheMiss(const OmMipMeshCoord &meshCoord);
	void HandleFetchUpdate();
	bool InitializeFetchThread();
	
	string mDirectoryPath;
	bool mMeshDataBuilt;

	//gl context to load mesh vbos
	QGLContext* mFetchThreadContext;


	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};









#pragma mark 
#pragma mark Serialization
/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmMipMeshManager, 0)

template<class Archive>
void 
OmMipMeshManager::serialize(Archive & ar, const unsigned int file_version) {

	ar & mDirectoryPath;
	ar & mMeshDataBuilt;
}




#endif
