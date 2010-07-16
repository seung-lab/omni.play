#include "FlatSurface.hpp"
#include "zi/vector/Vec.hpp"
#include "zi/base/base.h"
#include "HalfEdge.hpp"

#include <map>
#include <list>

namespace zi {

using namespace std;
using namespace zi::vector;

void FlatSurface(map<int, Mesh<vector::Vec3d> > &meshes,
                 int nX, int nY, int nZ,
                 double dX, double dY, double dZ,
                 int* points)
{

  map<int, Mesh<vector::Vec3d> > meshes2;
  int YxZ = (nY+1)*(nZ+1);
  int ni = 0, nj = 0, ind;
  int here, tox, toy, toz;

  Vec3d v[8];

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
        ind = ni + nj + k;

        here = points[ind];
        if (here != 0) {
          tox  = points[ind + YxZ];
          if (tox != here)
            meshes[here].addFace4(v[1], v[5], v[6], v[2]);

          toy  = points[ind + (nZ+1)];
          if (toy != here)
            meshes[here].addFace4(v[4], v[7], v[6], v[5]);

          toz  = points[ind + 1];
          if (toz != here)
            meshes[here].addFace4(v[3], v[2], v[6], v[7]);
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
    cout << "DONE: " << i << endl;
  }

  ni = 0, nj = 0;

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
        ind = ni + nj + k;

        here = points[ind];
        tox  = points[ind + YxZ];
        if (tox != 0)
          if (here != tox)
            meshes2[tox].addFace4(v[1], v[2], v[6], v[5]);

          toy  = points[ind + (nZ+1)];
          if (toy != 0)
            if (here != toy)
              meshes2[toy].addFace4(v[4], v[5], v[6], v[7]);

          toz  = points[ind + 1];
          if (toz != 0)
            if (here != toz)
            meshes2[toz].addFace4(v[3], v[7], v[6], v[2]);

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
    cout << "DONE: " << i << endl;
  }


  map<int, list<HalfEdge<Vec3d> *> > toUpdate;


  FOR_EACH (mit, meshes) {
    FOR_EACH (it, mit->second.edgesMap_) {
      if (!it->second->pair_) {
        HalfEdge<Vec3d> *opp =
          meshes2[mit->first].getEdge(it->first.second->point_,
                                      it->first.first->point_);
        assert(opp->pair_ == 0);
        pairUp(opp, it->second);
        toUpdate[mit->first].push_back(it->second);
      }
    }
  }

  FOR_EACH (mit, meshes) {
    FOR_EACH (it, toUpdate[mit->first]) {
      replaceVertex((*it)->vertex_,
                    mit->second.verticesMap_[(*it)->vertex_->point_]);

      replaceVertex((*it)->pair_->vertex_,
                    mit->second.verticesMap_[(*it)->pair_->vertex_->point_]);

      replaceVertex(mit->second.verticesMap_[(*it)->vertex_->point_],
                    mit->second.verticesMap_[(*it)->vertex_->point_]);

      replaceVertex(mit->second.verticesMap_[(*it)->pair_->vertex_->point_],
                    mit->second.verticesMap_[(*it)->pair_->vertex_->point_]);

      meshes2[mit->first].verticesMap_.erase((*it)->vertex_->point_);
      meshes2[mit->first].verticesMap_.erase((*it)->pair_->vertex_->point_);
    }
  }

  FOR_EACH (mit, meshes) {
    mit->second.verticesMap_.clear();
    mit->second.edgesMap_.clear();
    FOR_EACH (it, mit->second.faces_) {
      FOR_AROUND_FACE (e, (*it)) {
        mit->second.verticesMap_[e->vertex_->point_] = e->vertex_;
        mit->second.edgesMap_[make_pair(e->pair_->vertex_,
                                        e->vertex_)] = e;
      }
    }
    FOR_EACH (it, meshes2[mit->first].faces_) {
      mit->second.faces_.push_back(*it);
      FOR_AROUND_FACE (e, (*it)) {
        mit->second.verticesMap_[e->vertex_->point_] = e->vertex_;
        mit->second.edgesMap_[make_pair(e->pair_->vertex_,
                                        e->vertex_)] = e;
      }
    }
    mit->second.clearMaps();
  }

}

}
