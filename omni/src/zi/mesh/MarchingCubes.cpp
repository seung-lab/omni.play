/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#include "MarchingCubes.hpp"
#include "MarchingCubesTables.h"
#include "zi/vector/Vec.hpp"
#include "zi/base/base.h"
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include <inttypes.h>
#include <map>
#include <vector>

namespace zi {

using namespace std;
using namespace zi::vector;
using boost::shared_ptr;
using boost::unordered_map;
using boost::tuple;

#define MC_INTERP(p1, p2, val)                                          \
  ((vals[p1] == val) ^ (vals[p2] == val)) ? (v[p1]+v[p2])/2.0 : v[p1]


void MarchingCubes(std::map<int, SimpleMesh<vector::Vec3d> > &meshes,
                   int nX, int nY, int nZ,
                   double dX, double dY, double dZ,
                   int* points)
{

  map<int, int> here;

  int YxZ = (nY+1)*(nZ+1);
  int ni = 0, nj = 0, ind;
  int vals[8];

  Vec3d v[8];
  Vec3d vPtr[12];

  v[0] = Vec3d(0,  0,  0 );
  v[1] = Vec3d(dX, 0,  0 );
  v[2] = Vec3d(dX, 0,  dZ);
  v[3] = Vec3d(0,  0,  dZ);
  v[4] = Vec3d(0,  dY, 0 );
  v[5] = Vec3d(dX, dY, 0 );
  v[6] = Vec3d(dX, dY, dZ);
  v[7] = Vec3d(0,  dY, dZ);

  for(int i=0; i < nX; i++)  {
    for(int j=0; j < nY; j++)  {
      for(int k=0; k < nZ; k++) {

        here.clear();
        ind = ni + nj + k;

        vals[0] = points[ind];
        vals[1] = points[ind + YxZ];
        vals[2] = points[ind + YxZ + 1];
        vals[3] = points[ind + 1];
        vals[4] = points[ind + (nZ+1)];
        vals[5] = points[ind + YxZ + (nZ+1)];
        vals[6] = points[ind + YxZ + (nZ+1) + 1];
        vals[7] = points[ind + (nZ+1) + 1];

        for (int q = 0; q < 8; q++)
          if (vals[q] > 0)
            here[vals[q]] = 1;

        FOR_EACH (it, here) {

          int cIndex(0);
          for(int n=0; n < 8; n++)
            if (vals[n] != it->first)
              cIndex |= (1 << n);

          if (!edgeTable[cIndex])
            continue;


          if (edgeTable[cIndex] & 1   ) vPtr[0]  = MC_INTERP(0, 1, it->first);
          if (edgeTable[cIndex] & 2   ) vPtr[1]  = MC_INTERP(1, 2, it->first);
          if (edgeTable[cIndex] & 4   ) vPtr[2]  = MC_INTERP(2, 3, it->first);
          if (edgeTable[cIndex] & 8   ) vPtr[3]  = MC_INTERP(3, 0, it->first);
          if (edgeTable[cIndex] & 16  ) vPtr[4]  = MC_INTERP(4, 5, it->first);
          if (edgeTable[cIndex] & 32  ) vPtr[5]  = MC_INTERP(5, 6, it->first);
          if (edgeTable[cIndex] & 64  ) vPtr[6]  = MC_INTERP(6, 7, it->first);
          if (edgeTable[cIndex] & 128 ) vPtr[7]  = MC_INTERP(7, 4, it->first);
          if (edgeTable[cIndex] & 256 ) vPtr[8]  = MC_INTERP(0, 4, it->first);
          if (edgeTable[cIndex] & 512 ) vPtr[9]  = MC_INTERP(1, 5, it->first);
          if (edgeTable[cIndex] & 1024) vPtr[10] = MC_INTERP(2, 6, it->first);
          if (edgeTable[cIndex] & 2048) vPtr[11] = MC_INTERP(3, 7, it->first);

          for (int n=0; triTable[cIndex][n] != -1; n+=3) {
            meshes[it->first].addFace3(vPtr[triTable[cIndex][n+2]],
                                       vPtr[triTable[cIndex][n+1]],
                                       vPtr[triTable[cIndex][n]]);
          }

        }

        for (int z=0;z<8;z++) v[z][2] += dZ;

      }
      for (int z=0;z<8;z++) v[z][1] += dY;
      for (int z=0;z<8;z++) v[z][2]  = (double)((int)((z%4)/2))*dZ;
      nj += nZ + 1;
    }

    for (int z=0;z<8;z++) v[z][0] += dX;
    for (int z=0;z<8;z++) v[z][1]  = (double)((int)(z/4))*dY;
    ni += YxZ;
    nj  = 0;
  }

}

#define IVEC3(_x, _y, _z)                       \
  ((_x << 33) | (_y << 17) | (_z << 1))

#define Q_INTERP(p1, p2, val)                                           \
  ((vals[p1] == val) ^ (vals[p2] == val)) ? (v[p1]+v[p2]) >> 1 : v[p1]


boost::shared_ptr<boost::unordered_map<int, QuickMesh> >
QuickMarchingCubes(int inX, int inY, int inZ, int* points)
{

  int64_t nX = inX, nY = inY, nZ = inZ;

  boost::shared_ptr<boost::unordered_map<int, QuickMesh> >
    ret(new boost::unordered_map<int, QuickMesh>);

  map<int, int> here;

  int YxZ = (nY+1)*(nZ+1);
  int ni = 0, nj = 0, ind;
  int vals[8];

  int64_t v[8];
  int64_t vPtr[12];

  v[0] = IVEC3(0LL,  0LL,  0LL);
  v[1] = IVEC3(1LL,  0LL,  0LL);
  v[2] = IVEC3(1LL,  0LL,  1LL);
  v[3] = IVEC3(0LL,  0LL,  1LL);
  v[4] = IVEC3(0LL,  1LL,  0LL);
  v[5] = IVEC3(1LL,  1LL,  0LL);
  v[6] = IVEC3(1LL,  1LL,  1LL);
  v[7] = IVEC3(0LL,  1LL,  1LL);

  for(int i=0; i < nX; i++)  {
    for(int j=0; j < nY; j++)  {
      for(int k=0; k < nZ; k++) {

        here.clear();
        ind = ni + nj + k;

        vals[0] = points[ind];
        vals[1] = points[ind + YxZ];
        vals[2] = points[ind + YxZ + 1];
        vals[3] = points[ind + 1];
        vals[4] = points[ind + (nZ+1)];
        vals[5] = points[ind + YxZ + (nZ+1)];
        vals[6] = points[ind + YxZ + (nZ+1) + 1];
        vals[7] = points[ind + (nZ+1) + 1];

        for (int q = 0; q < 8; q++)
          if (vals[q] > 0)
            here[vals[q]] = 1;

        FOR_EACH (it, here) {

          int cIndex(0);
          for(int n=0; n < 8; n++)
            if (vals[n] != it->first)
              cIndex |= (1 << n);

          if (!edgeTable[cIndex])
            continue;


          if (edgeTable[cIndex] & 1   ) vPtr[0]  = Q_INTERP(0, 1, it->first);
          if (edgeTable[cIndex] & 2   ) vPtr[1]  = Q_INTERP(1, 2, it->first);
          if (edgeTable[cIndex] & 4   ) vPtr[2]  = Q_INTERP(2, 3, it->first);
          if (edgeTable[cIndex] & 8   ) vPtr[3]  = Q_INTERP(3, 0, it->first);
          if (edgeTable[cIndex] & 16  ) vPtr[4]  = Q_INTERP(4, 5, it->first);
          if (edgeTable[cIndex] & 32  ) vPtr[5]  = Q_INTERP(5, 6, it->first);
          if (edgeTable[cIndex] & 64  ) vPtr[6]  = Q_INTERP(6, 7, it->first);
          if (edgeTable[cIndex] & 128 ) vPtr[7]  = Q_INTERP(7, 4, it->first);
          if (edgeTable[cIndex] & 256 ) vPtr[8]  = Q_INTERP(0, 4, it->first);
          if (edgeTable[cIndex] & 512 ) vPtr[9]  = Q_INTERP(1, 5, it->first);
          if (edgeTable[cIndex] & 1024) vPtr[10] = Q_INTERP(2, 6, it->first);
          if (edgeTable[cIndex] & 2048) vPtr[11] = Q_INTERP(3, 7, it->first);

          for (int n=0; triTable[cIndex][n] != -1; n+=3) {
            (*ret)[it->first].push_back(tuple<int64_t, int64_t, int64_t>
                                        (vPtr[triTable[cIndex][n+2]],
                                         vPtr[triTable[cIndex][n+1]],
                                         vPtr[triTable[cIndex][n]]));
          }

        }

        for (int64_t z=0;z<8;z++) v[z] += 2LL;

      }

      for (int64_t z=0;z<8;z++) v[z] += 0x20000LL;
      for (int64_t z=0;z<8;z++) v[z]  = (v[z] & 0xFFFFFFFF0000LL) | (z & 2);
      nj += nZ + 1;
    }

    for (int64_t z=0;z<8;z++) v[z] += 0x200000000LL;
    for (int64_t z=0;z<8;z++) v[z]  = (v[z] & 0xFFFF0000FFFFLL) | ((z & 4) << 15);
    ni += YxZ;
    nj  = 0;

  }

  return ret;

}


#undef MC_INTERP
#undef Q_INTERP

}
