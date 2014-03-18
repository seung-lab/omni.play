#pragma once
#include "precomp.h"

#include "mesh/mesh.hpp"
#include "system/omOpenGLGarbageCollector.hpp"

namespace om {
namespace mesh {

class DisplayListMesh {
 public:
  DisplayListMesh(std::shared_ptr<IMesh> meshData, QGLContext const* context)
      : meshData_(meshData), displayList_(0), context_(context) {}

  ~DisplayListMesh() {
    OmOpenGLGarbageCollector::AddDisplayListID(context_, displayList_);
  }

  inline void Draw() {
    if (!displayList_) {
      createDisplayList();
      meshData_.reset();
    }
    glCallList(displayList_);
  }

 private:
  inline void createDisplayList() {
    if (!meshData_) {
      throw InvalidOperationException("Trying to draw missing Mesh Data.");
    }

    displayList_ = glGenLists(1);

    glNewList(displayList_, GL_COMPILE);

    meshData_->Draw();

    glEndList();
  }

  std::shared_ptr<IMesh> meshData_;
  GLuint displayList_;
  QGLContext const* context_;
};
}
}  // namespace om::mesh::
