#ifndef OM_MIP_MESH_MANAGER_H
#define OM_MIP_MESH_MANAGER_H

/*
 *
 *
 */

#include "segment/omSegmentPointers.h"
#include "system/cache/omMeshCache.h"

class OmMeshSource;
class OmMipChunk;
class OmMipChunkCoord;
class OmViewGroupState;
class QGLContext;

class OmMipMeshManager {
public:
	OmMipMeshManager();
	~OmMipMeshManager();

	void CloseDownThreads();
	
	//accessors
	const QString& GetDirectoryPath() const;
	void SetDirectoryPath(const QString &);
	
	//meshing
	OmMipMesh* AllocMesh(const OmMipMeshCoord &coord );
	void GetMesh(QExplicitlySharedDataPointer<OmMipMesh> &p_value, const OmMipMeshCoord &coord );
	void UncacheMesh(const OmMipMeshCoord &coord );
		
private:
	OmMeshCache *const mDataCache;
	
	void HandleFetchUpdate();
	
	QString mDirectoryPath;

	//gl context to load mesh vbos
	QGLContext* mFetchThreadContext;

	friend QDataStream &operator<<(QDataStream & out, const OmMipMeshManager & mm );
	friend QDataStream &operator>>(QDataStream & in, OmMipMeshManager & mm );
};

#endif
