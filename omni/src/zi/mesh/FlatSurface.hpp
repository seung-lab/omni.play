#ifndef FLATSURFACE_H_
#define FLATSURFACE_H_

#include "HalfEdge.hpp"
#include "zi/vector/Vec.hpp"
#include <map>

namespace zi {

void FlatSurface(std::map<int, Mesh<vector::Vec3d> > &meshes,
                 int nX, int nY, int nZ,
                 double dX, double dY, double dZ,
                 int* points);

}

#endif
