#ifndef OM_MIP_MESH_H
#define OM_MIP_MESH_H

/**
 *	The mesh data associated with the region of a segment enclosed
 *   by a specific MipChunk.
 *
 *	Brett Warne - bwarne@mit.edu - 3/7/09
 */

#include "common/omCommon.h"
#include "common/omGl.h"
#include "mesh/omMeshTypes.h"
#include "mesh/omMipMeshCoord.h"

class OmDataForMeshLoad;
class OmMeshCache;
class OmMipMeshManager;
class OmSegmentation;

class OmMipMesh {
public:
	OmMipMesh(OmSegmentation*, const OmMipMeshCoord&,
			  OmMipMeshManager*, OmMeshCache*);

	virtual ~OmMipMesh();

	void Load();
	void Flush(){}

	bool HasData() const;
	uint64_t NumBytes() const;

	void Draw();

private:
	OmSegmentation *const segmentation_;
	OmMeshCache *const cache_;
	OmMipMeshManager *const meshMan_;

	const OmMipMeshCoord mMeshCoordinate;

	boost::shared_ptr<OmDataForMeshLoad> data_;

	GLuint mVertexDataVboId;
	GLuint mVertexIndexDataVboId;
	bool isVbo();
	void createVbo();
	void deleteVbo();
	GLuint createVbo(const void *data, int dataSize,
					 GLenum target, GLenum usage);

 	GLuint displayList_;
	bool hasDisplayList_;
	void makeDisplayList();

	uint64_t numBytes_;
	bool hasData_;
};

#endif
