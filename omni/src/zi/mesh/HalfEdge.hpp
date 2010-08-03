/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef HALFEDGE_HPP
#define HALFEDGE_HPP

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <inttypes.h>
#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>

#include "zi/base/base.h"
#include "zi/vector/Vec.hpp"
#include "zi/trees/lib/FHeap.hpp"
#include "zi/mesh/ext/TriStrip/TriStripper.h"

namespace zi {

#define QuickMesh std::vector<boost::tuple<int64_t,int64_t,int64_t> > /* */
#define Qpt_x(_p) ((double)(_p >> 32))
#define Qpt_y(_p) ((double)((_p >> 16) & 0xFFFFLL))
#define Qpt_z(_p) ((double)(_p & 0xFFFFLL))

template<typename T>
struct HalfEdge;

template<typename T>
struct Face;

template<typename T>
struct Vertex {
  T            point_  ;
  T            normal_ ;
  bool         border_ ;
  HalfEdge<T> *edge_ ;
  Vertex() {}
  Vertex(T point): point_(point), border_(0), edge_(0) {}
  Vertex(T point, HalfEdge<T> *edge): point_(point), border_(0), edge_(edge) {}
  friend bool operator==(const Vertex &v1, const Vertex &v2) {
    return v1.point_ == v2.point_;
  }
  friend bool operator<(const Vertex &v1, const Vertex &v2) {
    return v1.point_ < v2.point_;
  }
};

template<typename T>
struct Face {
  HalfEdge<T> *edge_ ;
  Face(): edge_(0) {}
  Face(HalfEdge<T> *edge): edge_(edge) {}
  T realNormal() {
    return zi::vector::cross
      (edge_->next_->vertex_->point_ - edge_->vertex_->point_,
       edge_->next_->next_->vertex_->point_ - edge_->vertex_->point_);
  }
  T getNormal() {
    T n = zi::vector::cross
      (edge_->next_->vertex_->point_ - edge_->vertex_->point_,
       edge_->next_->next_->vertex_->point_ - edge_->vertex_->point_);
    n.normalize();
    return n;
  }
};

template<typename T>
struct HalfEdge : zi::Trees::FHeapable<double> {
  Vertex<T>   *vertex_;
  HalfEdge<T> *pair_  ;
  Face<T>     *face_  ;
  HalfEdge<T> *next_  ;
  T           *attachment_;
  ~HalfEdge() { if (attachment_ != 0) delete attachment_; }
  HalfEdge() : vertex_(0), pair_(0), attachment_(0) {}
  HalfEdge(Vertex<T> *vertex):
    vertex_(vertex), pair_(0), face_(0), attachment_(0) {}
  void deleteAttachment() {
    if (attachment_ != 0)
      delete attachment_;
    attachment_ = 0;
  }
};

template<typename T>
void pairUp(HalfEdge<T> *e1, HalfEdge<T> *e2)
{
  assert(e1 != e2);
  //assert(e1->vertex_ != e2->vertex_);
  //assert(e1->face_   != e2->face_);
  //assert(e1->pair_ == 0);
  //assert(e2->pair_ == 0);
  e1->pair_ = e2;
  e2->pair_ = e1;
  e1->vertex_->edge_ = e2;
  e2->vertex_->edge_ = e1;
}

#define PAIR_UP(e1, e2)                         \
  do {                                          \
    (e1)->pair_ = (e2);                         \
    (e2)->pair_ = (e1);                         \
    (e1)->vertex_->edge_ = (e2);                \
    (e2)->vertex_->edge_ = (e1);                \
  } while(0)


template<typename T>
HalfEdge<T> *createEdge(Vertex<T> *v1, Vertex<T> *v2)
{
  HalfEdge<T> *e1 = new HalfEdge<T>(v1);
  HalfEdge<T> *e2 = new HalfEdge<T>(v2);
  pairUp(e1, e2);
  return e2;
}

template<typename T>
Face<T> *createFace3(HalfEdge<T> *e1, HalfEdge<T> *e2, HalfEdge<T> *e3)
{
  e1->next_ = e2;
  e2->next_ = e3;
  e3->next_ = e1;
  e1->face_ = e2->face_ = e3->face_ = new Face<T>(e1);
  return e1->face_;
}

template<typename T>
Face<T> *createFace4(HalfEdge<T> *e1, HalfEdge<T> *e2,
                     HalfEdge<T> *e3, HalfEdge<T> *e4)
{
  e1->next_ = e2;
  e2->next_ = e3;
  e3->next_ = e4;
  e4->next_ = e1;
  e1->face_ = e2->face_ = e3->face_ = e4->face_ = new Face<T>(e1);
  return e1->face_;
}

template<typename T>
Face<T> *createFace3(Vertex<T> *v1, Vertex<T> *v2, Vertex<T> *v3)
{
  HalfEdge<T> *e1 = new HalfEdge<T>(v1);
  HalfEdge<T> *e2 = new HalfEdge<T>(v2);
  HalfEdge<T> *e3 = new HalfEdge<T>(v3);
  return createFace3(e1, e2, e3);
}

#define FOR_AROUND_VERTEX(it_var, vertex)                       \
  for (__typeof__(((vertex)->edge_)) it_var = (vertex)->edge_,  \
         __tmp  = 0; __tmp != it_var;                           \
       it_var = it_var->pair_->next_, __tmp = (vertex)->edge_)

#define FOR_AROUND_FACE(it_var, face)                           \
  for (__typeof__(((face)->edge_)) it_var = (face)->edge_,      \
         __tmp  = 0; __tmp != it_var;                           \
       it_var = it_var->next_, __tmp = (face)->edge_)


template<typename T>
void replaceVertex(Vertex<T> *v, Vertex<T> *newV)
{
  FOR_AROUND_VERTEX(e, v) {
    e->pair_->vertex_ = newV;
    assert(newV != e->vertex_);
  }
}


template<typename T>
void collapseEdge(HalfEdge<T> *edge)
{
  Vertex<T> *v1 = edge->pair_->vertex_;
  Vertex<T> *v2 = edge->vertex_;

  assert(v1 != edge->vertex_);
  FOR_AROUND_VERTEX(e, v1) {
    assert(e->vertex_ != v1);
  }

  edge->pair_->vertex_ = 0;
  replaceVertex(v2, v1);
  edge->pair_->vertex_ = v1;
  v2->edge_ = 0;

  pairUp(edge->next_->pair_, edge->next_->next_->pair_);
  pairUp(edge->pair_->next_->pair_, edge->pair_->next_->next_->pair_);

  FOR_AROUND_VERTEX(e, v1) {
    assert(e->vertex_ != v1);
  }

  edge->face_->edge_ = 0;
  edge->pair_->face_->edge_ = 0;
  edge->next_->next_->vertex_ = 0;
  edge->next_->vertex_ = 0;
  edge->vertex_ = 0;
  edge->pair_->next_->next_->vertex_ = 0;
  edge->pair_->next_->vertex_ = 0;
  edge->pair_->vertex_ = 0;
}


template<typename T>
struct Triangle {
  T v1, v2, v3;
  Triangle(const T &vx1, const T &vx2, const T &vx3)
    : v1(vx1), v2(vx2), v3(vx3) {}
};

template<typename T>
struct SimpleMesh {
  std::vector<Triangle<T>*> triangles_;
  void addFace3(const T &v1, const T &v2, const T &v3) {
    triangles_.push_back(new Triangle<T>(v1, v2, v3));
  }
  ~SimpleMesh() { clear(); }
  void clear() {
    FOR_EACH(it, triangles_)
      delete (*it);
    triangles_.clear();
  }
};

template<typename T>
struct Mesh {
  std::list<Vertex<T>* >                   vertices_   ;
  std::list<HalfEdge<T>* >                 edges_      ;
  std::map<T, Vertex<T>* >                 verticesMap_;
  std::map<std::pair<Vertex<T>*, Vertex<T>* >, HalfEdge<T>* > edgesMap_   ;
  std::list<Face<T> *>                     faces_      ;
  std::map<Vertex<T>*, T>                  vertexNormals_;
  Mesh(): vertices_(), edges_(), verticesMap_(), edgesMap_() {}

  ~Mesh() {
    FOR_EACH(it, vertices_)
      delete (*it);

    FOR_EACH(it, edges_)
      delete (*it);

    FOR_EACH(it, faces_)
      delete (*it);
  }

  void toStrips(Tri::tri_stripper::primitives_vector *x,
                std::vector<Vertex<T> *> &vx) {
    int lastIdx = 0;
    std::map<Vertex<T>*, int> inds;
    Tri::tri_stripper::indices triIndices;

    FOR_EACH(itf, faces_) {
      if ((*itf)->edge_ != 0) {

        zi::Face<T> *f = (*itf);

        FOR_AROUND_FACE(fe, f) {

          ITERATOR(inds) it = inds.find(fe->vertex_);
          if (it == inds.end()) {
            triIndices.push_back(lastIdx);
            inds[fe->vertex_] = lastIdx++;
            fe->vertex_->normal_.normalize();
            vx.push_back(fe->vertex_);
          } else {
            triIndices.push_back(it->second);
          }

        }
      }
    }
    Tri::tri_stripper stripper(triIndices);
    stripper.SetMinStripSize(3);
    stripper.Strip(x);
  }

  Vertex<T> *addVertex(const T &v) {
    if (verticesMap_.find(v) == verticesMap_.end()) {
      verticesMap_[v] = new Vertex<T>(v);
    }
    return verticesMap_[v];
  }

  Vertex<T> *getVertex(const T &v) {
    if (verticesMap_.find(v) == verticesMap_.end()) {
      return 0;
    }
    return verticesMap_[v];
  }

  void checkRep() {

    FOR_EACH (it, vertices_) {
      assert((*it)->edge_->pair_->vertex_ == (*it));
    }
    FOR_EACH (it, edges_) {

      assert((*it)->pair_ != 0);
      assert((*it)->pair_ != (*it));

      assert(((*it)->face_->edge_ == (*it)) ||
             ((*it)->face_->edge_->next_ == (*it)) ||
             ((*it)->face_->edge_->next_->next_ == (*it)));

      bool b = false;
      FOR_AROUND_VERTEX(e, (*it)->vertex_)
        b = b || (e->pair_ == (*it));

      assert(b);

      assert((*it)->pair_->pair_ == (*it));
      assert((*it)->next_->next_->next_ == (*it));
      assert((*it)->vertex_ == (*it)->next_->pair_->vertex_);
      assert((*it)->vertex_ != (*it)->pair_->vertex_);
    }
    FOR_EACH (it, faces_) {
      assert((*it)->edge_->face_ == (*it));
      assert((*it)->edge_->next_->face_ == (*it));
      assert((*it)->edge_->next_->next_->face_ == (*it));
    }
  }

  bool addFace3(Vertex<T> *vx1, Vertex<T> *vx2, Vertex<T> *vx3) {
    HalfEdge<T> *e1, *e2, *e3;

    if (edgesMap_.find(std::make_pair(vx3, vx1)) != edgesMap_.end()) {
      return false;
    } else {
      e1 = new HalfEdge<T>(vx1);
      edgesMap_[std::make_pair(vx3, vx1)] = e1;
    }

    if (edgesMap_.find(std::make_pair(vx1, vx2)) != edgesMap_.end()) {
      return false;
    } else {
      e2 = new HalfEdge<T>(vx2);
      edgesMap_[std::make_pair(vx1, vx2)] = e2;
    }

    if (edgesMap_.find(std::make_pair(vx2, vx3)) != edgesMap_.end()) {
      return false;
    } else {
      e3 = new HalfEdge<T>(vx3);
      edgesMap_[std::make_pair(vx2, vx3)] = e3;
    }

    Face<T> *face = createFace3<T>(e1, e2, e3);
    faces_.push_back(face);

    if (edgesMap_.find(std::make_pair(vx1, vx3)) != edgesMap_.end()) {
      pairUp<T>(e1, edgesMap_[std::make_pair(vx1, vx3)]);
    }

    if (edgesMap_.find(std::make_pair(vx2, vx1)) != edgesMap_.end()) {
      pairUp<T>(e2, edgesMap_[std::make_pair(vx2, vx1)]);
    }

    if (edgesMap_.find(std::make_pair(vx3, vx2)) != edgesMap_.end()) {
      pairUp<T>(e3, edgesMap_[std::make_pair(vx3, vx2)]);
    }

    return true;
  }

  bool addFace4(Vertex<T> *vx1, Vertex<T> *vx2,
                Vertex<T> *vx3, Vertex<T> *vx4) {

    HalfEdge<T> *e1, *e2, *e3, *e4;

    if (edgesMap_.find(std::make_pair(vx4, vx1)) != edgesMap_.end()) {
      return false;
    } else {
      e1 = new HalfEdge<T>(vx1);
      edgesMap_[std::make_pair(vx4, vx1)] = e1;
    }

    if (edgesMap_.find(std::make_pair(vx1, vx2)) != edgesMap_.end()) {
      return false;
    } else {
      e2 = new HalfEdge<T>(vx2);
      edgesMap_[std::make_pair(vx1, vx2)] = e2;
    }

    if (edgesMap_.find(std::make_pair(vx2, vx3)) != edgesMap_.end()) {
      return false;
    } else {
      e3 = new HalfEdge<T>(vx3);
      edgesMap_[std::make_pair(vx2, vx3)] = e3;
    }

    if (edgesMap_.find(std::make_pair(vx3, vx4)) != edgesMap_.end()) {
      return false;
    } else {
      e4 = new HalfEdge<T>(vx4);
      edgesMap_[std::make_pair(vx3, vx4)] = e4;
    }

    Face<T> *face = createFace4<T>(e1, e2, e3, e4);
    faces_.push_back(face);

    if (edgesMap_.find(std::make_pair(vx1, vx4)) != edgesMap_.end()) {
      pairUp<T>(e1, edgesMap_[std::make_pair(vx1, vx4)]);
    }

    if (edgesMap_.find(std::make_pair(vx2, vx1)) != edgesMap_.end()) {
      pairUp<T>(e2, edgesMap_[std::make_pair(vx2, vx1)]);
    }

    if (edgesMap_.find(std::make_pair(vx3, vx2)) != edgesMap_.end()) {
      pairUp<T>(e3, edgesMap_[std::make_pair(vx3, vx2)]);
    }

    if (edgesMap_.find(std::make_pair(vx4, vx3)) != edgesMap_.end()) {
      pairUp<T>(e4, edgesMap_[std::make_pair(vx4, vx3)]);
    }

    return true;
  }

  bool addFace3(const T &v1, const T &v2, const T &v3) {

    Vertex<T> *vx1 = addVertex(v1);
    Vertex<T> *vx2 = addVertex(v2);
    Vertex<T> *vx3 = addVertex(v3);
    return addFace3(vx1, vx2, vx3);

  }

  HalfEdge<T> *getEdge(const T &v1, const T &v2) {
    Vertex<T> *vx1 = getVertex(v1);
    Vertex<T> *vx2 = getVertex(v2);
    if (vx1 && vx2) {
      ITERATOR (edgesMap_) it = edgesMap_.find(make_pair(vx1, vx2));
      if (it != edgesMap_.end()) {
        return it->second;
      }
    }
    return 0;
  }

  bool addFace4(const T &v1, const T &v2, const T &v3, const T &v4) {

    Vertex<T> *vx1 = addVertex(v1);
    Vertex<T> *vx2 = addVertex(v2);
    Vertex<T> *vx3 = addVertex(v3);
    Vertex<T> *vx4 = addVertex(v4);
    return addFace4(vx1, vx2, vx3, vx4);

  }

  void clearMaps() {
    FOR_EACH (it, edgesMap_) {
      edges_.push_back(it->second);
      if (it->second->pair_ == 0) {
        HalfEdge<T> *pair = new HalfEdge<T>(it->second->next_->next_->vertex_);
        pair->next_ = pair;
        pair->vertex_->border_ = it->second->vertex_->border_ = true;
        pairUp(it->second, pair);
      }
      assert(it->second->pair_ != 0);
      assert(it->second->pair_->pair_ == it->second);
    }
    edgesMap_.clear();

    FOR_EACH (it, faces_) {
      (*it)->edge_->vertex_->edge_ = (*it)->edge_->next_;
    }

    FOR_EACH (it, verticesMap_) {
      if (it->second->edge_ != 0) {
        vertices_.push_back(it->second);
        assert(it->second->edge_->pair_->vertex_ == it->second);
      }
    }
    verticesMap_.clear();
  }


  void loadFromFile(const std::string &filename, int delta = 1) {

    std::ifstream fin(filename.c_str());
    std::cout << "Reading file: " << filename << std::endl;

    std::vector<Vertex<T> *> vmap;
    std::string line, _type;
    double x, y, z;
    int    a, b, c;

    while (getline(fin, line)) {

      std::stringstream ss(line);
      ss >> _type ;

      if (ss.eof())
        continue;

      if (_type == "v") {
        ss >> x >> y >> z;
        T vec(x, y, z);
        Vertex<T> *nv = new Vertex<T>(vec);
        vertices_.push_back(nv);
        vmap.push_back(nv);
      } else if (_type == "f") {
        ss >> a >> b >> c;
        if (!addFace3(vmap[a-delta], vmap[b-delta], vmap[c-delta])) {
          assert(false);
        }
      }

    }

    std::cout << "DONE!\n";
    calculateVertexNormals();
    checkRep();

  }

  void calculateVertexNormals() {

    //vertexNormals_.clear();

    FOR_EACH (it, vertices_)
      (*it)->normal_ = T(0);
      //vertexNormals_[*it] = T(0);

    FOR_EACH (it, faces_) {
      T normal = (*it)->getNormal();
      T center = ((*it)->edge_->vertex_->point_ +
                  (*it)->edge_->next_->vertex_->point_ +
                  (*it)->edge_->next_->vertex_->point_) / 3.0;
      FOR_AROUND_FACE (e, (*it)) {
        e->vertex_->normal_ += normal * (e->vertex_->point_ - center).sqrlen();
      }
    }
  }

  void fromSimpleMesh(SimpleMesh<T> &sm) {
    FOR_EACH(it, sm.triangles_)
      addFace3((*it)->v1, (*it)->v2, (*it)->v3);
    sm.clear();
  }

  void fromQuickMesh(QuickMesh &qm, double dX, double dY, double dZ) {
    dX /= 2.0; dY /= 2.0; dZ /= 2.0;
    boost::unordered_map<int64_t, Vertex<T>* > vertices;
    FOR_EACH(it, qm) {
      int64_t v[3] = {it->template get<0>(), it->template get<1>(), it->template get<2>()};
      for (int i=0;i<3;++i)
        if (vertices.find(v[i]) == vertices.end()) {
          Vertex<T> *newV = new Vertex<T>(T(Qpt_x(v[i]) * dX,
                                            Qpt_y(v[i]) * dY,
                                            Qpt_z(v[i]) * dZ));
          vertices.insert(std::make_pair(v[i], newV));
          vertices_.push_back(newV);
        }
      addFace3(vertices[v[0]], vertices[v[1]], vertices[v[2]]);
    }
  }

};

#undef Qpt_x
#undef Qpt_y
#undef Qpt_z


}

#endif
