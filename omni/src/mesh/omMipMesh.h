#ifndef OM_MIP_SEGMENT_MESH_H
#define OM_MIP_SEGMENT_MESH_H

/**
 *	The mesh data associated with the region of a segment enclosed by a specific MipChunk.
 *
 *	Brett Warne - bwarne@mit.edu - 3/7/09
 */

#include "common/omCommon.h"
#include "common/omGl.h"
#include "datalayer/omDataWrapper.h"
#include "mesh/omMeshTypes.h"
#include "mesh/omMipMeshCoord.h"

class OmHdf5;
class OmMipMesh;
class OmSegmentManager;
class OmMipMeshManager;
class OmMeshCache;
class ziMeshingChunk;
class ziMesher;
class MipChunkMeshCollector;

class OmMipMesh {

public:
	OmMipMesh(const OmMipMeshCoord &id, OmMipMeshManager *, OmMeshCache *);
	virtual ~OmMipMesh();

	void Load();
	void Save();
	uint64_t NumBytes() const {
		return numBytes_;
	}
	void Flush(){}

	std::string GetDirectoryPath();

	bool IsVbo();
	void CreateVbo();
	void DeleteVbo();

	bool IsEmptyMesh();

	void Draw();

	bool hasData(){
		return mHasData;
	}

private:
	OmMeshCache *const cache_;
	OmMipMeshManager *const mpMipMeshManager;
	OmMipMeshCoord mMeshCoordinate;

	bool mHasData;
	std::string mPath;

	void SetNumBytes();
	uint64_t numBytes_;

	// interleved strip offset (into vertex data) and strip size data
	uint32_t mStripCount;
	OmDataWrapperPtr mpStripOffsetSizeDataWrap; //dim = 2 * mStripCount

	uint32_t mTrianCount;
	OmDataWrapperPtr mpTrianOffsetSizeDataWrap; //dim = 2 * mTrianCount

	// offsets for vectors in geometry data (specifies geometry)
	uint32_t mVertexIndexCount;
	OmDataWrapperPtr mpVertexIndexDataWrap; //dim = mVertexIndexCount with 2 bytes check 65K limit

	uint32_t m2VertexIndexCount;
	GLuint *mp2VertexIndexData;

	// interleved vertex and normal data (raw data)
	uint32_t mVertexCount;
	OmDataWrapperPtr mpVertexDataWrap;	//dim = 6 * mVertexCount 4 bytes

	GLuint mVertexDataVboId;
	GLuint mVertexIndexDataVboId;

	uint32_t m2VertexCount;
	GLfloat *m2VertexData;

 	GLuint displayList_;
	bool hasDisplayList_;
	GLuint createVbo(const void *data, int dataSize, GLenum target, GLenum usage);
	void makeDisplayList();

	void doLoad();

	friend class OmMesher;
	friend class ziMeshingChunk;
	friend class ziMesher;
    friend class MipChunkMeshCollector;
};

#endif
