#include "utility/omHdf5Manager.h"
#include "omMipMesh.h"
#include "omMipMeshManager.h"

#include "segment/omSegmentManager.h"
#include "system/omProjectData.h"
#include "system/omLocalPreferences.h"
#include "project/omProject.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace bfs = boost::filesystem;

#include <fstream>
#include "common/omDebug.h"
#include <QFile>

#define DEBUG 0

static const char *MIP_MESH_FILE_NAME = "mesh.%d.dat";

//utility
GLuint createVbo(const void *data, int dataSize, GLenum target, GLenum usage);

/////////////////////////////////
///////
///////          MipMesh
///////

OmMipMesh::OmMipMesh(const OmMipMeshCoord & id, OmMipMeshManager * pMipMeshManager)
:OmCacheableBase(pMipMeshManager), mpMipMeshManager(pMipMeshManager), mMeshCoordinate(id)
{

	//init mesh data
	mStripCount = 0;
	mpStripOffsetSizeData = NULL;

	mVertexIndexCount = 0;
	mpVertexIndexData = NULL;

	mVertexCount = 0;
	mpVertexData = NULL;

	mVertexDataVboId = NULL_VBO_ID;
	mVertexIndexDataVboId = NULL_VBO_ID;

	mHdf5File = NULL;

	mSegmentationID = 0;
}

OmMipMesh::~OmMipMesh()
{

	//debug("FIXME", << "OmMipMesh::~OmMipMesh()" << endl;

	//if was vbo, then delete vbos 
	if (IsVbo()) {
		DeleteVbo();

		//assert this local data was erased
		assert(mpVertexIndexData == NULL);
		assert(mpVertexData == NULL);
	}

	//meshes with no data don't have alloc'd data
	if (mpStripOffsetSizeData) {
		delete[]mpStripOffsetSizeData;
		mpStripOffsetSizeData = NULL;
	}

	if (mpVertexIndexData) {
		delete[]mpVertexIndexData;
		mpVertexIndexData = NULL;
	}

	if (mpVertexData) {
		delete[]mpVertexData;
		mpVertexData = NULL;
	}

	if (mHdf5File) {
		delete mHdf5File;
	}
}

/////////////////////////////////
///////          Mesh IO Methods

void OmMipMesh::Load()
{
	int size;

	//read meta data
	OmHdf5Path fpath;
	fpath.setPath( GetDirectoryPath() + "metamesh.dat" );
	char *meta = (char *)OmProjectData::ReadRawData(fpath);
	char result = *meta;
	delete meta;

	//if meta is zero, then no data so skip
	if (!result)
		return;

	//read strip offset/size data
	fpath.setPath( GetDirectoryPath() + "stripoffset.dat" );
	mpStripOffsetSizeData = (uint32_t *) OmProjectData::ReadRawData(fpath, &size);
	mStripCount = size / (2 * sizeof(uint32_t));

	//read vertex offset data
	fpath.setPath( GetDirectoryPath() + "vertexoffset.dat" );
	mpVertexIndexData = (GLuint *) OmProjectData::ReadRawData(fpath, &size);
	mVertexIndexCount = size / sizeof(GLuint);

	//read strip offset/size data
	fpath.setPath( GetDirectoryPath() + "vertex.dat" );
	mpVertexData = (GLfloat *) OmProjectData::ReadRawData(fpath, &size);
	mVertexCount = size / (6 * sizeof(GLfloat));

	//debug("genone","OmMipMesh::Load: got mesh from disk\n");
}

string OmMipMesh::GetLocalPathForHd5fChunk()
{
        char mip_dname_buf[MAX_FNAME_SIZE];
	QString pid = OmStateManager::getPID();
	string ret;
        sprintf(mip_dname_buf, "%d.%d.%d_%d_%d.%s.%s.h5",
               	getSegmentationID(),
                mMeshCoordinate.MipChunkCoord.Level,
                mMeshCoordinate.MipChunkCoord.Coordinate.x,
                mMeshCoordinate.MipChunkCoord.Coordinate.y,
                mMeshCoordinate.MipChunkCoord.Coordinate.z,
               	qPrintable(OmProject::GetFileName()),
		qPrintable(pid));

        ret = string(qPrintable(OmLocalPreferences::getScratchPath())) + "/meshinator_" + string(mip_dname_buf);
	debug("parallel", "parallel mesh fs path: %s\n", ret.c_str());
        return ret;
}

void OmMipMesh::Save()
{
	OmHdf5 * hdf5File = NULL;

	if (OmLocalPreferences::getStoreMeshesInTempFolder() || 
	    OmStateManager::getParallel()) {
		hdf5File = OmHdf5Manager::getOmHdf5File( QString::fromStdString( GetLocalPathForHd5fChunk() ) );
		hdf5File->create();
	} else {
		hdf5File = OmProjectData::GetHdf5File();
	}

	int size;

	assert(hdf5File);

	//write meta data
	OmHdf5Path fpath;
	fpath.setPath( GetDirectoryPath() + "metamesh.dat" );
	char meta = ((mStripCount && mVertexIndexCount && mVertexCount) != false);
	hdf5File->dataset_raw_create_tree_overwrite(fpath, 1, &meta);

	//if meta is zero then skip mesh
	if (!meta)
		return;

	//write strip offset/size data
	fpath.setPath( GetDirectoryPath() + "stripoffset.dat" );
	size = 2 * mStripCount * sizeof(uint32_t);
	hdf5File->dataset_raw_create_tree_overwrite(fpath, size, mpStripOffsetSizeData);

	//write vertex offset data
	fpath.setPath( GetDirectoryPath() + "vertexoffset.dat" );
	size = mVertexIndexCount * sizeof(GLuint);
	hdf5File->dataset_raw_create_tree_overwrite(fpath, size, mpVertexIndexData);

	//write strip offset/size data
	fpath.setPath( GetDirectoryPath() + "vertex.dat" );
	size = 6 * mVertexCount * sizeof(GLfloat);
	hdf5File->dataset_raw_create_tree_overwrite(fpath, size, mpVertexData);
}

string OmMipMesh::GetFileName()
{
	char mip_dname_buf[MAX_FNAME_SIZE];
	sprintf(mip_dname_buf, MIP_MESH_FILE_NAME, mMeshCoordinate.DataValue);
	return string(mip_dname_buf);
}

string OmMipMesh::GetDirectoryPath()
{

	//use mesh coord to construct rest of path
	char mip_dname_buf[MAX_FNAME_SIZE];
	sprintf(mip_dname_buf, "%d/%d_%d_%d/mesh/%d/",
		mMeshCoordinate.MipChunkCoord.Level,
		mMeshCoordinate.MipChunkCoord.Coordinate.x,
		mMeshCoordinate.MipChunkCoord.Coordinate.y,
		mMeshCoordinate.MipChunkCoord.Coordinate.z, mMeshCoordinate.DataValue);

	return mpMipMeshManager->GetDirectoryPath() + string(mip_dname_buf);
}

bool OmMipMesh::IsEmptyMesh()
{
	return (0 == mVertexCount);
}

/////////////////////////////////
///////          VBO Methods

bool OmMipMesh::IsVbo()
{
	return (NULL_VBO_ID != mVertexDataVboId) || (NULL_VBO_ID != mVertexIndexDataVboId);
}

void OmMipMesh::CreateVbo()
{
	//debug("genone","OmMipMesh::CreateVbo()\n");

	//ignore empty meshes
	if (IsEmptyMesh())
		return;

	//should not already be vbo
	if (IsVbo())
		assert(false);

	//create the VBO for the vertex data
	//2 (pos/norm) * 3 (x/y/z) * sizeof(GLfloat)
	//debug("genone","OmMipMesh::CreateVbo(): vertex data");
	int vertex_data_size = 6 * mVertexCount * sizeof(GLfloat);
	mVertexDataVboId = createVbo(mpVertexData, vertex_data_size, GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);

	//create VBO for the vertex index data
	//debug("genone","OmMipMesh::CreateVbo(): vertex index data\n");
	int vertex_index_data_size = mVertexIndexCount * sizeof(GLuint);
	mVertexIndexDataVboId = createVbo(mpVertexIndexData, vertex_index_data_size,
					  GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);

	//debug("genone","OmMipMesh::CreateVbo(): delete local\n");
	//delete local data
	delete[]mpVertexData;
	mpVertexData = NULL;

	delete[]mpVertexIndexData;
	mpVertexIndexData = NULL;

	//debug("genone","OmMipMesh::CreateVbo(): update size\n");
	//update cache
	UpdateSize(vertex_data_size + vertex_index_data_size);

	//debug("genone","OmMipMesh::CreateVbo: done\n");
}

void OmMipMesh::DeleteVbo()
{
	if (!IsVbo())
		assert(false);

	glDeleteBuffersARB(1, &mVertexDataVboId);
	glDeleteBuffersARB(1, &mVertexIndexDataVboId);

	//update cache
	int vertex_data_size = 6 * mVertexCount * sizeof(GLfloat);
	int vertex_index_data_size = mVertexIndexCount * sizeof(GLuint);
	UpdateSize(-(vertex_data_size + vertex_index_data_size));
}

/////////////////////////////////
///////          Draw Methods

bool OmMipMesh::Draw(bool doCreateVbo)
{
	bool ret = false;

	//ignore empty meshes
	if (IsEmptyMesh())
		return ret;

	//if(!IsVbo()) assert(false);
	if (!IsVbo()) {
		debug("vbo", "going to create vbo\n");
		if (doCreateVbo) {
			CreateVbo();
			ret = true;
		} else {
			debug("vbo", "not creating vbo\n");
			return ret;
		}
		
		debug("vbo", "done to creating vbo\n");
	}

	//debug("genone","OmMipMesh::Draw()");

	////bind VBOs so gl*Pointer() operations are offset instead of real pointers
	//bind vertex data VBO
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVertexDataVboId);
	//specify vector size for interleaved vector data
	uint32_t vector_size = 3 * sizeof(GL_FLOAT);
	//specify normal (type, stride, pointer)
	glNormalPointer(GL_FLOAT, 2 * vector_size, (void *)vector_size);
	//specify vertex (coordinates, type, stride, pointer)
	glVertexPointer(3, GL_FLOAT, 2 * vector_size, 0);

	////bind vertex index data VBO
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mVertexIndexDataVboId);
	//specify index pointer (type, stride, pointer)
	glIndexPointer(GL_UNSIGNED_INT, 0, 0);

	//activate client state vertex and normal array
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	//// draw mesh elements
	debug("elements", "going to draw elements\n");
	for (uint32_t idx = 0; idx < mStripCount; idx++) {
		glDrawElements(GL_TRIANGLE_STRIP,	//triangle strip
			       mpStripOffsetSizeData[2 * idx + 1],	//elements in strip
			       GL_UNSIGNED_INT,	//type
			       (GLuint *) 0 + mpStripOffsetSizeData[2 * idx]);	//strip offset
	}
	debug("elements", "done drawing %i elements\n", mStripCount);

	//disable client state
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	// release VBOs: gl*Pointer() return to normal
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, NULL_VBO_ID);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NULL_VBO_ID);

	return ret;
}

/////////////////////////////////
///////          ostream

ostream & operator<<(ostream & out, const OmMipMesh & m)
{
	out << "Strip Count: \t" << m.mStripCount << "\n";
	out << "Vertex Index Count: \t" << m.mVertexIndexCount << "\n";
	out << "Vertex Count: \t" << m.mVertexCount << "\n";
	return out;
}

/////////////////////////////////
///////          Utility Functions

/*
 * Creates a VBO with given properties and checks it was loaded properly.
 * If it is not completely loaded, it deletes itself.
 *
 * http://www.songho.ca/opengl/gl_vbo.html
 */
GLuint createVbo(const void *data, int dataSize, GLenum target, GLenum usage)
{
	//debug("genone","createVbo()\n");

	// 0 is reserved, glGenBuffersARB() will return non-zero id if success
	GLuint id = NULL_VBO_ID;

	glGenBuffersARB(1, &id);	// create a vbo
	glBindBufferARB(target, id);	// activate vbo id to use
	glBufferDataARB(target, dataSize, data, usage);	// upload data to video card

	// check data size in VBO is same as input array, if not return 0 and delete VBO
	int bufferSize = 0;
	glGetBufferParameterivARB(target, GL_BUFFER_SIZE_ARB, &bufferSize);
	if (dataSize != bufferSize) {
		glDeleteBuffersARB(1, &id);
		id = NULL_VBO_ID;
		//throw OmChunkSegment3dMeshException("Not enough memory to load VBO.");
	}
	//bwarne: unbind
	glBindBufferARB(target, NULL_VBO_ID);

	return id;		// return VBO id
}
