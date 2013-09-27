#include "common/common.h"
#include "mesh/degenerateStrip/mesh.h"
#include "utility/glInclude.h"

namespace om {
namespace mesh {

static const GLuint NULL_VBO_ID = 0;

struct DegenerateStripMeshImpl {
  GLuint DegenerateStripDataVboId;

  DegenerateStripMeshImpl() { DegenerateStripDataVboId = NULL_VBO_ID; }
  ~DegenerateStripMeshImpl() {
    if (isVbo()) {
      deleteVbo();
    }
  }

  bool isVbo() { return NULL_VBO_ID != DegenerateStripDataVboId; }

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

  bool createVbo(const std::vector<Vector3f>& data) {
    if (isVbo()) {
      deleteVbo();
    }

    DegenerateStripDataVboId =
        createVbo(data.data(), data.size() * sizeof(Vector3f),
                  GL_ARRAY_BUFFER_ARB, GL_STATIC_DRAW_ARB);

    if (!DegenerateStripDataVboId) {
      return false;
    }

    return true;
  }

  void deleteVbo() {
    if (!isVbo()) {
      throw IoException("not a vbo");
    }

    glDeleteBuffersARB(1, &DegenerateStripDataVboId);
    DegenerateStripDataVboId = NULL_VBO_ID;
  }
};

DegenerateStripMesh::DegenerateStripMesh(const coords::Mesh& coord)
    : impl_(new DegenerateStripMeshImpl()), coord_(coord) {}

void DegenerateStripMesh::Draw() {
  if (!impl_->isVbo()) {
    if (!impl_->createVbo(data_)) {
      log_errors(Mesh) << "Failed to create VBO";
      return;
    }
  }

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // bind vertex data VBO
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, impl_->DegenerateStripDataVboId);

  // specify vector size for interleaved vector data
  size_t vector_size = 3 * sizeof(GL_FLOAT);

  // specify normal (type, stride, pointer)
  glNormalPointer(GL_FLOAT, 2 * vector_size, (void*)vector_size);

  // specify vertex (coordinates, type, stride, pointer)
  glVertexPointer(3, GL_FLOAT, 2 * vector_size, 0);

  // activate client state vertex and normal array
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  // draw mesh elements
  glDrawArrays(GL_TRIANGLE_STRIP, 0, data_.size() / 2);

  // disable client state
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  // release VBOs: gl*Pointer() return to normal
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, NULL_VBO_ID);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NULL_VBO_ID);

  glPopAttrib();
}
}
size_t size(const mesh::DegenerateStripMesh& m) {
  return m.Data().size() * sizeof(Vector3f);
}

}  // namespace om::mesh::
