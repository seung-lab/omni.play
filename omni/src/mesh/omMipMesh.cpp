#include "common/omDebug.h"
#include "mesh/omMipMesh.h"
#include "mesh/omMipMeshManager.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "system/omLocalPreferences.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "system/omGarbage.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"

#include <QFile>

/////////////////////////////////
///////          MipMesh

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

OmMipMesh::OmMipMesh(const OmMipMeshCoord & id, OmMipMeshManager * pMipMeshManager, OmMeshCache * cache)
  : OmCacheableBase(cache)
  , mpMipMeshManager(pMipMeshManager)
  , mMeshCoordinate(id)
{
  mHasData = false;
  displayList = 0;
  hasDisplayList = false;
  //init mesh data
  mTrianCount = 0;

  mStripCount = 0;

  mpTrianOffsetSizeDataWrap = OmDataWrapperInvalid();
  mpStripOffsetSizeDataWrap = OmDataWrapperInvalid();
  mpVertexIndexDataWrap = OmDataWrapperInvalid();
  mpVertexDataWrap = OmDataWrapperInvalid();

  mVertexIndexCount = 0;

  mVertexCount = 0;

  mVertexDataVboId = NULL_VBO_ID;
  mVertexIndexDataVboId = NULL_VBO_ID;

  mHdf5File = NULL;

  mSegmentationID = 0;

  mPath = GetDirectoryPath();
}

OmMipMesh::~OmMipMesh()
{
  if (hasDisplayList) {
    hasDisplayList = false;
    OmGarbage::asOmGenlistId(displayList);
  }

  if (mHdf5File) {
    delete mHdf5File;
  }

  // OmDataWrapperPtr will take care of image data destruction...
}

/////////////////////////////////
///////          Mesh IO Methods

void OmMipMesh::Load()
{
  try {
    doLoad();
  } catch (...) {
  }
}

void OmMipMesh::doLoad()
{
  OmDataPath fpath( mPath + "metamesh.dat" );
  if( !OmProjectData::GetProjectDataReader()->dataset_exists( fpath ) ){
    return;
  }

  OmDataWrapperPtr result = OmProjectData::GetProjectDataReader()->readDataset(fpath);

  unsigned char noData = *(result->getPtr<unsigned char>());

  //if meta is zero, then no data so skip
  if ( 0 == noData ){
    mHasData = false;
    return;
  }

  mHasData = true;
  int size;

  //read triangles offset/size data  (uint32_t *)
  fpath.setPath( mPath + "trianoffset.dat" );
  mpTrianOffsetSizeDataWrap = OmProjectData::GetProjectDataReader()->readDataset(fpath, &size);
  mTrianCount = size / (2 * sizeof(uint32_t));

  //read strip offset/size data  (uint32_t *)
  fpath.setPath( mPath + "stripoffset.dat" );
  mpStripOffsetSizeDataWrap = OmProjectData::GetProjectDataReader()->readDataset(fpath, &size);
  mStripCount = size / (2 * sizeof(uint32_t));

  //read vertex offset data (GLuint *)
  fpath.setPath( mPath + "vertexoffset.dat" );
  mpVertexIndexDataWrap = OmProjectData::GetProjectDataReader()->readDataset(fpath, &size);
  mVertexIndexCount = size / sizeof(GLuint);

  //read strip offset/size data (GLfloat *)
  fpath.setPath( mPath + "vertex.dat" );
  mpVertexDataWrap = OmProjectData::GetProjectDataReader()->readDataset(fpath, &size);
  mVertexCount = size / (6 * sizeof(GLfloat));

  int vertex_data_size = 6 * mVertexCount * sizeof(GLfloat);
  int vertex_index_data_size = mVertexIndexCount * sizeof(GLuint);

  //update cache
  UpdateSize(vertex_data_size + vertex_index_data_size);
}

void OmMipMesh::Save()
{
  OmIDataWriter* hdf5File = NULL;

  if (OmLocalPreferences::getStoreMeshesInTempFolder() || OmStateManager::getParallel()) {
    const std::string path = OmDataPaths::getLocalPathForHd5fChunk(mMeshCoordinate,
								   mSegmentationID);
    hdf5File = OmDataLayer::getWriter(QString::fromStdString(path), false);
  } else {
    hdf5File = OmProjectData::GetDataWriter();
  }

  int size;

  assert(hdf5File);

  //write meta data
  OmDataPath fpath;
  fpath.setPath( mPath + "metamesh.dat" );
  char meta = ((mStripCount && mVertexIndexCount && mVertexCount) != false);
  hdf5File->writeDataset(fpath, 1, OmDataWrapperRaw(&meta));

  //if meta is zero then skip mesh
  if (!meta)
    return;

  //write trian offset/size data
  fpath.setPath( mPath + "trianoffset.dat" );
  size = 2 * mTrianCount * sizeof(uint32_t);
  hdf5File->writeDataset(fpath, size, mpTrianOffsetSizeDataWrap);

  //write strip offset/size data
  fpath.setPath( mPath + "stripoffset.dat" );
  size = 2 * mStripCount * sizeof(uint32_t);
  hdf5File->writeDataset(fpath, size, mpStripOffsetSizeDataWrap);

  //write vertex offset data
  fpath.setPath( mPath + "vertexoffset.dat" );
  size = mVertexIndexCount * sizeof(GLuint);
  hdf5File->writeDataset(fpath, size, mpVertexIndexDataWrap);

  //write strip offset/size data
  fpath.setPath( mPath + "vertex.dat" );
  size = 6 * mVertexCount * sizeof(GLfloat);
  hdf5File->writeDataset(fpath, size, mpVertexDataWrap);

  if (OmLocalPreferences::getStoreMeshesInTempFolder() || OmStateManager::getParallel()) {
  }
}

string OmMipMesh::GetFileName()
{
  return OmDataPaths::getMeshFileName( mMeshCoordinate );
}

string OmMipMesh::GetDirectoryPath()
{
  return OmDataPaths::getMeshDirectoryPath(mMeshCoordinate,
					   mpMipMeshManager);
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
  mVertexDataVboId = createVbo(mpVertexDataWrap->getPtr<float>(), vertex_data_size, GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);

  //create VBO for the vertex index data
  //debug("genone","OmMipMesh::CreateVbo(): vertex index data\n");
  int vertex_index_data_size = mVertexIndexCount * sizeof(GLuint);
  mVertexIndexDataVboId = createVbo(mpVertexIndexDataWrap->getPtr<GLuint>(), vertex_index_data_size,
                                    GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);
}

void OmMipMesh::DeleteVbo()
{
  if (!IsVbo()) {
    assert(false);
  }

  glDeleteBuffersARB(1, &mVertexDataVboId);
  glDeleteBuffersARB(1, &mVertexIndexDataVboId);

  //update cache
  int vertex_data_size = 6 * mVertexCount * sizeof(GLfloat);
  int vertex_index_data_size = mVertexIndexCount * sizeof(GLuint);
  UpdateSize(-(vertex_data_size + vertex_index_data_size));

  mVertexDataVboId = NULL_VBO_ID;
  mVertexIndexDataVboId = NULL_VBO_ID;
}

/////////////////////////////////
///////          Draw Methods

bool OmMipMesh::Draw(bool)
{
  bool ret = false;

  //ignore empty meshes
  if (IsEmptyMesh()) {
    return ret;
  }

  if (!hasDisplayList) {
    displayList = glGenLists(1);
    hasDisplayList = true;
    glNewList(displayList, GL_COMPILE);

    CreateVbo();

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

    int size = 0;
    //// draw mesh elements
    debug("elements", "going to draw elements\n");
    if(mStripCount) {
      unsigned int * stripOffsetSizeData = mpStripOffsetSizeDataWrap->getPtr<unsigned int>();
      for (uint32_t idx = 0; idx < mStripCount; ++idx) {
        size += stripOffsetSizeData[2 * idx + 1];
        glDrawElements(GL_TRIANGLE_STRIP,	//triangle strip
                     stripOffsetSizeData[2 * idx + 1],	//elements in strip
                     GL_UNSIGNED_INT,	//type
                     (GLuint *) 0 + stripOffsetSizeData[2 * idx]);	//strip offset
      }
    }
    if(mTrianCount) {
      unsigned int * trianOffsetSizeData = mpTrianOffsetSizeDataWrap->getPtr<unsigned int>();
      for (uint32_t idx = 0; idx < mTrianCount; ++idx) {
 	glDrawElements(GL_TRIANGLES,      //triangle trian
              trianOffsetSizeData[2 * idx + 1],        //elements in trian
	      GL_UNSIGNED_INT,   //type
 	      (GLuint *) 0 + trianOffsetSizeData[2 * idx]);    //trian offset
      }
    }
    debug("tri","strip count: %i, avg: %f\n", mStripCount, (float)size / mStripCount);
    debug("elements", "done drawing %i elements\n", mStripCount);

    //disable client state
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    // release VBOs: gl*Pointer() return to normal
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, NULL_VBO_ID);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NULL_VBO_ID);

    DeleteVbo();

    glPopAttrib();
    glEndList();
  }

  glCallList(displayList);

  return ret;
}

/////////////////////////////////
///////          Utility Functions

/*
 * Creates a VBO with given properties and checks it was loaded properly.
 * If it is not completely loaded, it deletes itself.
 *
 * http://www.songho.ca/opengl/gl_vbo.html
 */
GLuint OmMipMesh::createVbo(const void *data, int dataSize, GLenum target, GLenum usage)
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
    printf("Not enough memory to load VBO\n");
    //throw OmChunkSegment3dMeshException("Not enough memory to load VBO.");
  }

  // unbind
  glBindBufferARB(target, NULL_VBO_ID);

  return id;		// return VBO id
}

void OmMipMesh::Flush()
{
}

void OmMipMesh::setSegmentationID(OmId sid)
{
  mSegmentationID = sid;
}

OmId OmMipMesh::getSegmentationID()
{
  return mSegmentationID;
}
