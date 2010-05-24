#ifndef OM_MIP_MESH_MANAGER_H
#define OM_MIP_MESH_MANAGER_H

/*
 *
 *
 */

#include "mesh/omMipMesh.h"
#include "mesh/omMipMeshCoord.h"
#include "segment/omSegment.h"
#include "common/omStd.h"
#include "system/omThreadedCache.h"

typedef OmThreadedCache< OmMipMeshCoord, OmMipMesh > MipMeshCache;

class OmMipChunk;
class OmMipChunkCoord;
class OmMeshSource;
class QGLContext;

class OmMipMeshManager : public MipMeshCache {

 public:
	OmMipMeshManager();
	~OmMipMeshManager();
	
	//accessors
	const QString& GetDirectoryPath() const;
	void SetDirectoryPath(const QString &);
	
	//meshing
	OmMipMesh* AllocMesh(const OmMipMeshCoord &coord );
	void GetMesh(QExplicitlySharedDataPointer<OmMipMesh> &p_value, const OmMipMeshCoord &coord );
	void UncacheMesh(const OmMipMeshCoord &coord );
	
	//drawing
	void DrawMeshes(const OmBitfield &drawOps,
			const OmMipChunkCoord &mipCoord,
			std::vector< OmSegment* > & segmentsToDraw );
		
 private:
	OmMipMesh* HandleCacheMiss(const OmMipMeshCoord &meshCoord);
	void HandleFetchUpdate();
	bool InitializeFetchThread();
	
	QString mDirectoryPath;

	//gl context to load mesh vbos
	QGLContext* mFetchThreadContext;

	friend QDataStream &operator<<(QDataStream & out, const OmMipMeshManager & mm );
	friend QDataStream &operator>>(QDataStream & in, OmMipMeshManager & mm );
};

#endif
