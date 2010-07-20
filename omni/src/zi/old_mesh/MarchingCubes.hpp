#ifndef MARCHINGCUBES_H_
#define MARCHINGCUBES_H_

#include "HalfEdge.hpp"
#include "MarchingCubesTables.h"
#include "zi/vector/Vec.hpp"
#include <map>

namespace zi {

void MarchingCubes(std::map<int, SimpleMesh<vector::Vec3d> > &meshes,
                   int nX, int nY, int nZ,
                   double dX, double dY, double dZ,
                   int* points);

}

#endif
