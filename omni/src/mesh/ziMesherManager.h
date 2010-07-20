#ifndef ZIMESHERMANAGER_H_
#define ZIMESHERMANAGER_H_

#include "common/omGl.h"

#include "omMipMeshManager.h"
#include "volume/omMipChunkCoord.h"
#include "zi/thread/Mutex.h"
#include "zi/thread/Monitor.h"
#include "zi/vector/Vec.hpp"
#include "zi/mesh/HalfEdge.hpp"
#include "zi/mesh/ext/TriStrip/TriStripper.h"
#include <boost/shared_ptr.hpp>

class StrippedMesh {
public:
  StrippedMesh() :
    totalVertices_(0),
    totalIndices_(0),
    totalStrips_(0),
    totalTrians_(0),
    monitor_(),
    waiting_(0),
    free_(true) {}

  virtual ~StrippedMesh() {}
  void addStrips(const Tri::tri_stripper::primitives_vector &ptrs,
                 const std::vector< zi::Vertex< zi::vector::Vec3d > *> &vertices,
                 zi::vector::Vec3d scale, zi::vector::Vec3d trans);
  int totalVertices_;
  int totalIndices_;
  int totalStrips_;
  int totalTrians_;
  zi::Threads::Monitor   monitor_;
  std::vector<GLfloat>  vertices_;
  std::vector<GLuint>   indices_;
  std::vector<uint32_t> strips_;
  std::vector<uint32_t> trians_;
  int waiting_;
  bool free_;
};

class GrowingMeshes {
public:
  GrowingMeshes(const OmMipChunkCoord&coor);
  virtual ~GrowingMeshes() {};

  void requireDelivery() { remainingDeliveries_++; }
  void receive(std::map<int, std::pair<Tri::tri_stripper::primitives_vector,
               std::vector< zi::Vertex< zi::vector::Vec3d > *> > > &stripified,
               zi::vector::Vec3d scale, zi::vector::Vec3d trans);

  boost::shared_ptr<std::map<int, boost::shared_ptr<StrippedMesh> > >
  getStrippedMeshes()
  { return strippeds_; }

  bool isDone() { return remainingDeliveries_ == 0; }
  OmMipChunkCoord getCoor() { return coor_; }

private:
  int remainingDeliveries_;
  zi::Threads::Mutex   mutex_;
  OmMipChunkCoord      coor_;
  boost::shared_ptr<std::map<int, boost::shared_ptr<StrippedMesh> > > strippeds_;
};

#endif
