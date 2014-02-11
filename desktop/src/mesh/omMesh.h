#pragma once

/**
 *  The mesh data associated with the region of a segment enclosed
 *   by a specific MipChunk.
 *
 *  Brett Warne - bwarne@mit.edu - 3/7/09
 */

#include "common/common.h"
#include "coordinates/mesh.h"
#include "utility/glInclude.h"
#include "mesh/omMeshTypes.h"

class OmDataForMeshLoad;
class OmMeshCache;
class OmMeshManager;
class OmSegmentation;
class QGLContext;

class OmMesh {
 public:
  OmMesh(OmSegmentation*, const om::coords::Mesh&, OmMeshManager*,
         OmMeshCache*);

  virtual ~OmMesh();

  void Load();

  bool HasData() const;
  uint64_t NumBytes() const;

  void Draw(QGLContext const* context);

  OmDataForMeshLoad* Data() { return data_.get(); }

 private:
  OmSegmentation* const segmentation_;
  OmMeshCache* const cache_;
  OmMeshManager* const meshMan_;

  const om::coords::Mesh meshCoord_;

  QGLContext const* context_;

  std::shared_ptr<OmDataForMeshLoad> data_;

  GLuint vertexDataVboId_;
  GLuint vertexIndexDataVboId_;
  bool isVbo();
  bool createVbo();
  void deleteVbo();
  GLuint createVbo(const void* data, int dataSize, GLenum target, GLenum usage);

  boost::optional<GLuint> displayList_;
  void makeDisplayList(QGLContext const* context);

  uint64_t numBytes_;
  bool hasData_;
};
