#ifndef QUADRATIC_ERROR_MESH_SIMPL_H_
#define QUADRATIC_ERROR_MESH_SIMPL_H_

#include "HalfEdge.hpp"
#include "zi/vector/Vec.hpp"
#include "zi/trees/lib/FHeap.hpp"

#include <map>

namespace zi {

class QuadraticErrorSimplifier {
public:
  QuadraticErrorSimplifier(Mesh<vector::Vec3d> &mesh);
  virtual ~QuadraticErrorSimplifier();

  int runQueue(int triangleNoLimit, double errorLimit, int queueSizeLimit = 0);

private:

  void generateQValues();
  void queueInit();
  bool addToQueue(HalfEdge<vector::Vec3d>* edge);
  int  popQueue();
  void collectMeshGarbage();

  Mesh<vector::Vec3d> &mesh_;
  zi::Trees::FHeap<double> queue_;
  //std::map<std::pair<double, HalfEdge<vector::Vec3d>* >, vector::Vec3d > queue_;
  //std::map<HalfEdge<vector::Vec3d>*, double> errValues_;
  std::map<Vertex<vector::Vec3d>*, vector::Mat4d> QValues_  ;
};

}

#endif
