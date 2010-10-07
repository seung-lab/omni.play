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

class OmMipMesh {

public:
	OmMipMesh(const OmMipMeshCoord &id, OmMipMeshManager *, OmMeshCache *);
	virtual ~OmMipMesh();

	void Load();
	void Save();

	std::string GetFileName();
	std::string GetDirectoryPath();
	std::string GetLocalPathForHd5fChunk();

	bool IsVbo();
	void CreateVbo();
	void DeleteVbo();

	bool IsEmptyMesh();

	void Draw();
	void setSegmentationID(OmId sid);
	OmId getSegmentationID();

	bool hasData(){ return mHasData; }

private:
	OmMeshCache *const cache_;
	OmId mSegmentationID;
	OmHdf5* mHdf5File;
	OmMipMeshManager *const mpMipMeshManager;
	OmMipMeshCoord mMeshCoordinate;

	bool mHasData;
	std::string mPath;

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

	GLuint createVbo(const void *data, int dataSize, GLenum target, GLenum usage);
 	GLuint displayList;
	bool hasDisplayList;

	void doLoad();

	friend class OmMesher;
	friend class ziMeshingChunk;
};

#endif
