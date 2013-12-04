#include "common/common.h"
#include "mesh/vertexIndex/mesh.h"
#include "utility/glInclude.h"

namespace om {
namespace mesh {

static const GLuint NULL_VBO_ID = 0;

struct VertexIndexMeshImpl {
  GLuint VertexDataVboId;
  GLuint VertexIndexDataVboId;

  VertexIndexMeshImpl() {
    VertexDataVboId = NULL_VBO_ID;
    VertexIndexDataVboId = NULL_VBO_ID;
  }

  ~VertexIndexMeshImpl() {
    if (isVbo()) {
      deleteVbo();
    }
  }

  bool isVbo() {
    return (NULL_VBO_ID != VertexDataVboId) ||
           (NULL_VBO_ID != VertexIndexDataVboId);
  }

  /*
   * Creates a VBO with given properties and checks it was loaded properly.
   * If it is not completely loaded, it deletes itself.
   *
   * http://www.songho.ca/opengl/gl_vbo.html
   */
  GLuint createVbo(const void* data, int dataSize, GLenum target,
                   GLenum usage) {
    // 0 is reserved, glGenBuffersARB() will return non-zero id if success
    GLuint id = NULL_VBO_ID;

    glGenBuffersARB(1, &id);                         // create a vbo
    glBindBufferARB(target, id);                     // activate vbo id to use
    glBufferDataARB(target, dataSize, data, usage);  // upload data to video
                                                     // card

    // check data size in VBO is same as input array, if not return 0 and delete
    // VBO
    int bufferSize = 0;
    glGetBufferParameterivARB(target, GL_BUFFER_SIZE_ARB, &bufferSize);
    if (dataSize != bufferSize) {
      glDeleteBuffersARB(1, &id);
      id = NULL_VBO_ID;
      log_errors(Mesh) << "Not enough memory to load VBO";
    }

    // unbind
    glBindBufferARB(target, NULL_VBO_ID);

    return id;
  }

  bool createVbo(const VertexIndexData& data) {
    if (isVbo()) {
      deleteVbo();
    }

    // create the VBO for the vertex data
    VertexDataVboId = createVbo(data.VertexData(), data.VertexDataNumBytes(),
                                GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);

    if (!VertexDataVboId) {
      return false;
    }

    // create VBO for the vertex index data
    VertexIndexDataVboId =
        createVbo(data.VertexIndex(), data.VertexIndexNumBytes(),
                  GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);

    if (!VertexIndexDataVboId) {
      return false;
    }

    return true;
  }

  void deleteVbo() {
    if (!isVbo()) {
      throw IoException("not a vbo");
    }

    glDeleteBuffersARB(1, &VertexDataVboId);
    glDeleteBuffersARB(1, &VertexIndexDataVboId);

    VertexDataVboId = NULL_VBO_ID;
    VertexIndexDataVboId = NULL_VBO_ID;
  }
};

VertexIndexMesh::VertexIndexMesh(const coords::Mesh& coord)
    : impl_(new VertexIndexMeshImpl()), coord_(coord) {}

void VertexIndexMesh::Draw() {
  if (!impl_->isVbo()) {
    if (!impl_->createVbo(data_)) {
      log_errors(Mesh) << "Failed to create VBO";
      return;
    }
  }

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  ////bind VBOs so gl*Pointer() operations are offset instead of real pointers

  // bind vertex data VBO
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, impl_->VertexDataVboId);

  // specify vector size for interleaved vector data
  size_t vector_size = 3 * sizeof(GL_FLOAT);

  // specify normal (type, stride, pointer)
  glNormalPointer(GL_FLOAT, 2 * vector_size, (void*)vector_size);

  // specify vertex (coordinates, type, stride, pointer)
  glVertexPointer(3, GL_FLOAT, 2 * vector_size, 0);

  ////bind vertex index data VBO
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, impl_->VertexIndexDataVboId);

  // specify index pointer (type, stride, pointer)
  glIndexPointer(GL_UNSIGNED_INT, 0, 0);

  // activate client state vertex and normal array
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  // draw mesh elements
  if (data_.StripDataCount()) {
    uint32_t* stripOffsetSizeData = data_.StripData();
    for (uint32_t idx = 0; idx < data_.StripDataCount(); ++idx) {
      glDrawElements(GL_TRIANGLE_STRIP,                 // triangle strip
                     stripOffsetSizeData[2 * idx + 1],  // elements in strip
                     GL_UNSIGNED_INT,                   // type
                     (GLuint*)0 + stripOffsetSizeData[2 * idx]);  // strip
                                                                  // offset
    }
  }

  // disable client state
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  // release VBOs: gl*Pointer() return to normal
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, NULL_VBO_ID);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NULL_VBO_ID);

  glPopAttrib();
}
}
size_t size(const mesh::VertexIndexMesh& m) { return m.Data().NumBytes(); }

}  // namespace om::mesh::
