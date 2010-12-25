#include "common/omDebug.h"
#include "common/omGl.h"
#include "mesh/io/omMeshConvertV1toV2.hpp"
#include "mesh/io/omMeshMetadata.hpp"
#include "mesh/io/v2/omMeshReaderV2.hpp"
#include "mesh/omMipMesh.h"
#include "mesh/omMipMeshManager.h"
#include "system/cache/omMeshCache.h"
#include "system/omGarbage.h"

static const GLuint NULL_VBO_ID = 0;

#ifdef __WIN32__
typedef void (*GLDELETEBUFFERS)(GLsizei n, const GLuint *buffers);
typedef void (*GLBINDBUFFER)(GLenum target, GLuint buffer);
typedef void (*GLGENBUFFERS)(GLsizei n, GLuint *buffers);
typedef void (*GLBUFFERDATA)(GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
typedef void (*GLGETBUFFERPARAIV)(GLenum target, GLenum pname, GLint *params);

extern GLDELETEBUFFERS glDeleteBuffersARBFunction;
extern GLBINDBUFFER glBindBufferARBFunction;
extern GLGENBUFFERS glGenBuffersARBFunction;
extern GLBUFFERDATA glBufferDataARBFunction;
extern GLGETBUFFERPARAIV glGetBufferParameterivARBFunction;

#define glDeleteBuffersARB glDeleteBuffersARBFunction
#define glBindBufferARB glBindBufferARBFunction
#define glGenBuffersARB glGenBuffersARBFunction
#define glBufferDataARB glBufferDataARBFunction
#define glGetBufferParameterivARB glGetBufferParameterivARBFunction;
#endif

OmMipMesh::OmMipMesh(OmSegmentation* seg,
					 const OmMipMeshCoord& coord,
					 OmMipMeshManager* pMipMeshManager,
					 OmMeshCache* cache)
	: segmentation_(seg)
	, cache_(cache)
	, meshMan_(pMipMeshManager)
	, mMeshCoordinate(coord)
	, displayList_(0)
	, hasDisplayList_(false)
	, numBytes_(0)
	, hasData_(false)
{
	mVertexDataVboId = NULL_VBO_ID;
	mVertexIndexDataVboId = NULL_VBO_ID;
}

OmMipMesh::~OmMipMesh()
{
	if (hasDisplayList_) {
		OmGarbage::assignOmGenlistId(displayList_);
	}
}

uint64_t OmMipMesh::NumBytes() const {
	return numBytes_;
}

bool OmMipMesh::HasData() const {
	return hasData_;
}

/////////////////////////////////
///////          VBO Methods

bool OmMipMesh::isVbo()
{
	return (NULL_VBO_ID != mVertexDataVboId) ||
		(NULL_VBO_ID != mVertexIndexDataVboId);
}

void OmMipMesh::createVbo()
{
	if(!hasData_){
		return;
	}

	if(isVbo()){
		throw OmIoException("should not already be vbo");
	}

	//create the VBO for the vertex data
	mVertexDataVboId = createVbo(data_->VertexData(),
								 data_->VertexDataNumBytes(),
								 GL_ARRAY_BUFFER_ARB,
								 GL_STATIC_DRAW_ARB);

	//create VBO for the vertex index data
	mVertexIndexDataVboId = createVbo(data_->VertexIndex(),
									  data_->VertexIndexNumBytes(),
									  GL_ARRAY_BUFFER_ARB,
									  GL_STATIC_DRAW_ARB);
}

void OmMipMesh::deleteVbo()
{
	if(!isVbo()){
		throw OmIoException("not a vbo");
	}

	glDeleteBuffersARB(1, &mVertexDataVboId);
	glDeleteBuffersARB(1, &mVertexIndexDataVboId);

	//update cache
	cache_->UpdateSize(-1. * data_->NumBytes());

	mVertexDataVboId = NULL_VBO_ID;
	mVertexIndexDataVboId = NULL_VBO_ID;
}

void OmMipMesh::makeDisplayList()
{
	displayList_ = glGenLists(1);
	hasDisplayList_ = true;
	glNewList(displayList_, GL_COMPILE);

	createVbo();

	glPushAttrib(GL_ALL_ATTRIB_BITS);

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

	// draw mesh elements
	if(data_->StripDataCount()) {
		uint32_t* stripOffsetSizeData = data_->StripData();
		for (uint32_t idx = 0; idx < data_->StripDataCount(); ++idx) {
			glDrawElements(GL_TRIANGLE_STRIP,	//triangle strip
						   stripOffsetSizeData[2 * idx + 1],	//elements in strip
						   GL_UNSIGNED_INT,	//type
						   (GLuint *) 0 + stripOffsetSizeData[2 * idx]);	//strip offset
		}
	}

	if(data_->TrianDataCount()) {
		uint32_t* trianOffsetSizeData = data_->TrianData();
		for (uint32_t idx = 0; idx < data_->TrianDataCount(); ++idx) {
			glDrawElements(GL_TRIANGLES, //triangle trian
						   trianOffsetSizeData[2 * idx + 1], //elements in trian
						   GL_UNSIGNED_INT,   //type
						   (GLuint *) 0 + trianOffsetSizeData[2 * idx]);    //trian offset
		}
	}

	//disable client state
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	// release VBOs: gl*Pointer() return to normal
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, NULL_VBO_ID);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NULL_VBO_ID);

	deleteVbo();

	glPopAttrib();
	glEndList();

	data_.reset();
}

void OmMipMesh::Draw()
{
	if(!hasData_){
		return;
	}

	if(!hasDisplayList_){
		makeDisplayList();
	}

	glCallList(displayList_);
}

/*
 * Creates a VBO with given properties and checks it was loaded properly.
 * If it is not completely loaded, it deletes itself.
 *
 * http://www.songho.ca/opengl/gl_vbo.html
 */
GLuint OmMipMesh::createVbo(const void *data, int dataSize, GLenum target, GLenum usage)
{
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
		printf("Not enough memory to load VBO\n");
	}

	// unbind
	glBindBufferARB(target, NULL_VBO_ID);

	return id;
}

void OmMipMesh::Load()
{
	OmMeshMetadata* metadata = meshMan_->Metadata();

	if(!metadata->IsBuilt()){
		return;
	}

	if(metadata->IsHDF5()){
		data_ = meshMan_->Converter()->ReadAndConvert(mMeshCoordinate);

	}else {
		data_ = meshMan_->Reader()->Read(mMeshCoordinate);
	}

	if(!data_){
		return;
	}

	numBytes_ = data_->NumBytes();
	hasData_ = data_->HasData();

	cache_->UpdateSize(numBytes_);
}
