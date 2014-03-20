// #include "utility/glInclude.h"
#include "common/logging.h"
#include "mesh/io/omMeshConvertV1toV2.hpp"
#include "mesh/io/omMeshMetadata.hpp"
#include "mesh/io/v2/omMeshReaderV2.hpp"
#include "mesh/omMesh.h"
#include "mesh/omMeshManager.h"
#include "system/cache/omMeshCache.h"
#include "system/omOpenGLGarbageCollector.hpp"

static const GLuint NULL_VBO_ID = 0;

OmMesh::OmMesh(OmSegmentation* seg, const om::coords::Mesh& coord,
               OmMeshManager* pMipMeshManager, OmMeshCache* cache)
    : segmentation_(seg),
      cache_(cache),
      meshMan_(pMipMeshManager),
      meshCoord_(coord),
      numBytes_(0),
      hasData_(false) {
  vertexDataVboId_ = NULL_VBO_ID;
  vertexIndexDataVboId_ = NULL_VBO_ID;
}

OmMesh::~OmMesh() {
  if (displayList_) {
    OmOpenGLGarbageCollector::AddDisplayListID(context_, *displayList_);
  }
}

uint64_t OmMesh::NumBytes() const { return numBytes_; }

bool OmMesh::HasData() const { return hasData_; }

/////////////////////////////////
///////          VBO Methods

bool OmMesh::isVbo() {
  return (NULL_VBO_ID != vertexDataVboId_) ||
         (NULL_VBO_ID != vertexIndexDataVboId_);
}

bool OmMesh::createVbo() {
  if (!hasData_) {
    return false;
  }

  if (isVbo()) {
    throw om::IoException("should not already be vbo");
  }

  // create the VBO for the vertex data
  vertexDataVboId_ = createVbo(data_->VertexData(), data_->VertexDataNumBytes(),
                               GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);

  if (!vertexDataVboId_) {
    return false;
  }

  // create VBO for the vertex index data
  vertexIndexDataVboId_ =
      createVbo(data_->VertexIndex(), data_->VertexIndexNumBytes(),
                GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);

  if (!vertexIndexDataVboId_) {
    return false;
  }

  return true;
}

void OmMesh::deleteVbo() {
  if (!isVbo()) {
    throw om::IoException("not a vbo");
  }

  glDeleteBuffersARB(1, &vertexDataVboId_);
  glDeleteBuffersARB(1, &vertexIndexDataVboId_);

  vertexDataVboId_ = NULL_VBO_ID;
  vertexIndexDataVboId_ = NULL_VBO_ID;
}

void OmMesh::makeDisplayList(QGLContext const* context) {
  context_ = context;

  displayList_ = glGenLists(1);

  glNewList(*displayList_, GL_COMPILE);

  if (!createVbo()) {
    return;
  }

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  ////bind VBOs so gl*Pointer() operations are offset instead of real pointers

  // bind vertex data VBO
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexDataVboId_);

  // specify vector size for interleaved vector data
  size_t vector_size = 3 * sizeof(GL_FLOAT);

  // specify normal (type, stride, pointer)
  glNormalPointer(GL_FLOAT, 2 * vector_size, (GLvoid*)vector_size);

  // specify vertex (coordinates, type, stride, pointer)
  glVertexPointer(3, GL_FLOAT, 2 * vector_size, 0);

  ////bind vertex index data VBO
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vertexIndexDataVboId_);

  // specify index pointer (type, stride, pointer)
  glIndexPointer(GL_UNSIGNED_INT, 0, 0);

  // activate client state vertex and normal array
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  // draw mesh elements
  if (data_->StripDataCount()) {
    uint32_t* stripOffsetSizeData = data_->StripData();
    for (uint32_t idx = 0; idx < data_->StripDataCount(); ++idx) {
      glDrawElements(
          GL_TRIANGLE_STRIP,                           // triangle strip
          stripOffsetSizeData[2 * idx + 1],            // elements in strip
          GL_UNSIGNED_INT,                             // type
          (GLuint*)0 + stripOffsetSizeData[2 * idx]);  // strip offset
    }
  }

  // old, VTK-based meshes
  if (data_->TrianDataCount()) {
    uint32_t* trianOffsetSizeData = data_->TrianData();
    for (uint32_t idx = 0; idx < data_->TrianDataCount(); ++idx) {
      glDrawElements(
          GL_TRIANGLES,                                // triangle trian
          trianOffsetSizeData[2 * idx + 1],            // elements in trian
          GL_UNSIGNED_INT,                             // type
          (GLuint*)0 + trianOffsetSizeData[2 * idx]);  // trian offset
    }
  }

  // disable client state
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

void OmMesh::Draw(QGLContext const* context) {
  if (!hasData_) {
    return;
  }

  if (!displayList_) {
    makeDisplayList(context);
  }

  glCallList(*displayList_);
}

/*
 * Creates a VBO with given properties and checks it was loaded properly.
 * If it is not completely loaded, it deletes itself.
 *
 * http://www.songho.ca/opengl/gl_vbo.html
 */
GLuint OmMesh::createVbo(const void* data, int dataSize, GLenum target,
                         GLenum usage) {
  // 0 is reserved, glGenBuffersARB() will return non-zero id if success
  GLuint id = NULL_VBO_ID;

  glGenBuffersARB(1, &id);                         // create a vbo
  glBindBufferARB(target, id);                     // activate vbo id to use
  glBufferDataARB(target, dataSize, data, usage);  // upload data to video card

  // check data size in VBO is same as input array, if not return 0 and delete
  // VBO
  int bufferSize = 0;
  glGetBufferParameterivARB(target, GL_BUFFER_SIZE_ARB, &bufferSize);
  if (dataSize != bufferSize) {
    glDeleteBuffersARB(1, &id);
    id = NULL_VBO_ID;
    log_infos << "Not enough memory to load VBO";
  }

  // unbind
  glBindBufferARB(target, NULL_VBO_ID);

  return id;
}

void OmMesh::Load() {
  OmMeshMetadata* metadata = meshMan_->Metadata();

  if (!metadata->IsBuilt()) {
    return;
  }

  if (metadata->IsHDF5()) {
    data_ = meshMan_->Converter()->ReadAndConvert(meshCoord_);

  } else {
    data_ = meshMan_->Reader()->Read(meshCoord_);
  }

  if (!data_) {
    return;
  }

  numBytes_ = data_->NumBytes();
  hasData_ = data_->HasData();
}
