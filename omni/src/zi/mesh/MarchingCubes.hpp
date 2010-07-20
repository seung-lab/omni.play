#ifndef MARCHINGCUBES_H_
#define MARCHINGCUBES_H_

#include "HalfEdge.hpp"
#include "MarchingCubesTables.h"
#include "zi/vector/Vec.hpp"
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <inttypes.h>
#include <map>
#include <vector>


namespace zi {

void MarchingCubes(std::map<int, SimpleMesh<vector::Vec3d> > &meshes,
                   int nX, int nY, int nZ,
                   double dX, double dY, double dZ,
                   int* points);

boost::shared_ptr<boost::unordered_map<int, QuickMesh> >
QuickMarchingCubes(int inX, int inY, int inZ, int* points);


}

#endif
