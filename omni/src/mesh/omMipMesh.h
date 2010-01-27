#ifndef OM_MIP_SEGMENT_MESH_H
#define OM_MIP_SEGMENT_MESH_H

/*
 *	The mesh data associated with the region of a segment enclosed by a specific MipChunk.
 *
 *	Brett Warne - bwarne@mit.edu - 3/7/09
 */

#include "omMeshTypes.h"

#include "omMipMeshCoord.h"
#include "common/omStd.h"
#include "common/omGl.h"
#include "common/omSerialization.h"
#include "system/omCacheableBase.h"

#include <boost/serialization/split_free.hpp>

class OmMipMesh;
class OmSegmentManager;
class OmMipMeshManager;

class OmMipMesh:protected OmCacheableBase {

 public:
	//OmMipMesh() : CacheableMesh(NULL), mSegmentManager(NULL) { };
	OmMipMesh(const OmMipMeshCoord & id, OmMipMeshManager * pMipMeshManager);
	~OmMipMesh();

	void Load();
	void Save();

	string GetFileName();
	string GetDirectoryPath();

	bool IsVbo();
	void CreateVbo();
	void DeleteVbo();

	bool IsEmptyMesh();

	void Draw();

 private:
	 OmMipMeshCoord mMeshCoordinate;
	OmMipMeshManager *const mpMipMeshManager;

	// interleved strip offset (into vertex data) and strip size data
	uint32_t mStripCount;
	uint32_t *mpStripOffsetSizeData;	//dim = 2 * mStripCount

	// offsets for vectors in geometry data (specifies geometry)
	uint32_t mVertexIndexCount;
	GLuint *mpVertexIndexData;	//dim = mVertexIndexCount 
	//with 2 bytes check 65K limit

	// interleved vertex and normal data (raw data)
	uint32_t mVertexCount;
	GLfloat *mpVertexData;	//dim = 6 * mVertexCount
	// 4 bytes

	GLuint mVertexDataVboId;
	GLuint mVertexIndexDataVboId;

	// ostream
	friend ostream & operator<<(ostream & out, const OmMipMesh & m);

	friend class OmMipChunkMesher;
	friend class OmMesher;

	//split serialization to allocate dynamic arrays
	friend class boost::serialization::access;

	 template < class Archive > void save(Archive & ar, const unsigned int version) const;

	 template < class Archive > void load(Archive & ar, const unsigned int version);

	 BOOST_SERIALIZATION_SPLIT_MEMBER()
};

#pragma mark
#pragma mark Serialization
/////////////////////////////////
///////          Serialization

BOOST_CLASS_VERSION(OmMipMesh, 0)

template < class Archive > void
 OmMipMesh::save(Archive & ar, const unsigned int version) const const
{
	ar & mMeshCoordinate;

	//mesh data
	ar & mStripCount;
	for (int i = 0; i < 2 * mStripCount; ++i)
		ar & mpStripOffsetSizeData[i];

	ar & mVertexIndexCount;
	for (int i = 0; i < mVertexIndexCount; ++i)
		ar & mpVertexIndexData[i];

	ar & mVertexCount;
	for (int i = 0; i < 6 * mVertexCount; ++i)
		ar & mpVertexData[i];
}

template < class Archive > void OmMipMesh::load(Archive & ar, const unsigned int version)
{
	ar & mMeshCoordinate;

	//alloc and read mesh data
	ar & mStripCount;
	mpStripOffsetSizeData = new uint32_t[2 * mStripCount];
	for (int i = 0; i < 2 * mStripCount; ++i)
		ar & mpStripOffsetSizeData[i];

	ar & mVertexIndexCount;
	mpVertexIndexData = new GLuint[mVertexIndexCount];
	for (int i = 0; i < mVertexIndexCount; ++i)
		ar & mpVertexIndexData[i];

	ar & mVertexCount;
	mpVertexData = new GLfloat[6 * mVertexCount];
	for (int i = 0; i < 6 * mVertexCount; ++i)
		ar & mpVertexData[i];
}

#endif
