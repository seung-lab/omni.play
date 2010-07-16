#ifndef MESH_SUBDIVISION_H_
#define MESH_SUBDIVISION_H_

#include "HalfEdge.hpp"
#include "zi/vector/Vec.hpp"
#include "zi/base/base.h"

#include <map>
#include <iostream>

namespace zi {

//using namespace zi::vector;

template <typename T>
class MeshSubdivision {
public:
  MeshSubdivision(Mesh<T> &mesh);
  virtual ~MeshSubdivision();

  int runQueue(int queueSizeLimit, double errorLimit);
  void subdivide();
  bool triangulate();
  bool checkRep();
private:
  struct EdgeDivision {
    Vertex<T>   *edgeVertex_;
    HalfEdge<T> *firstHalf_ ;
    HalfEdge<T> *secondHalf_ ;
    HalfEdge<T> *inGoing_ ;
    HalfEdge<T> *outGoing_ ;
    double       sharpness_;
    EdgeDivision() : edgeVertex_(0), firstHalf_(0),
                     secondHalf_(0), inGoing_(0),
                     outGoing_(0), sharpness_(-1.0) {}
  };
  struct VertexSubdivision {
    Vertex<T>   *nextVertex_;
    T sumF, sumE, sumS;
    int E, S;
    double smooth_;
    VertexSubdivision() :
      nextVertex_(0), sumF(0), sumE(0), sumS(0),
      E(0), S(0), smooth_(0.0) {}
  };

  void generateEdgeVertices();
  void generateFaceVertices();
  void generateVertexVertices();
  void splitHalfEdge(HalfEdge<T>* edge);

  Mesh<T> &mesh_;
  std::map<Face<T>*,     Vertex<T>* >        faceVertices_;
  std::map<HalfEdge<T>*, EdgeDivision >      edgeSubdivision_;
  std::map<Vertex<T>*,   VertexSubdivision > vertexSubdivision_;
  Mesh<T> *newMesh_;
};


template <typename T>
MeshSubdivision<T>::MeshSubdivision(Mesh<T> &mesh)
  : mesh_(mesh)
{
}

template <typename T>
MeshSubdivision<T>::~MeshSubdivision()
{
}

template <typename T>
void MeshSubdivision<T>::generateFaceVertices()
{
  //faceVertices_.clear();
  FOR_EACH (it, mesh_.faces_) {
    T sum(0);
    double count(0);
    FOR_AROUND_FACE (fIt, (*it)) {
      sum   += fIt->vertex_->point_;
      count += 1.0;
    }

    if (count > 0.0) {
      faceVertices_[*it] = new Vertex<T>(sum / count);
    } else {
      faceVertices_[*it] = new Vertex<T>(sum);
    }
    newMesh_->vertices_.push_back(faceVertices_[*it]);
  }
}

template <typename T>
void MeshSubdivision<T>::generateEdgeVertices()
{
  //edgeSubdivision_.clear();

  FOR_EACH (it, mesh_.faces_) {
    T sum(0);
    double count(4.0);

    FOR_AROUND_FACE (fIt, (*it)) {

      if (edgeSubdivision_[fIt].edgeVertex_ == 0) {

        if (edgeSubdivision_[fIt].sharpness_ <= 0.0) {
          sum = fIt->vertex_->point_ + fIt->pair_->vertex_->point_;

          if (fIt->pair_->face_ != 0) {
            sum += faceVertices_[fIt->pair_->face_]->point_;
            sum += faceVertices_[*it]->point_;
          } else {
            count = 2.0;
          }
          edgeSubdivision_[fIt].edgeVertex_ =
            edgeSubdivision_[fIt->pair_].edgeVertex_ =
            new Vertex<T>(sum/count);
          newMesh_->vertices_.push_back(edgeSubdivision_[fIt].edgeVertex_);
        } else {
          sum = fIt->vertex_->point_ + fIt->pair_->vertex_->point_;
          edgeSubdivision_[fIt].edgeVertex_ =
            edgeSubdivision_[fIt->pair_].edgeVertex_ =
            new Vertex<T>(sum/2.0);
          newMesh_->vertices_.push_back(edgeSubdivision_[fIt].edgeVertex_);
        }


      }
    }

  }
}

template <typename T>
void MeshSubdivision<T>::generateVertexVertices()
{
  //vertexSubdivision_.clear();

  FOR_EACH (it, mesh_.faces_) {
    FOR_AROUND_FACE (fIt, (*it)) {
      vertexSubdivision_[fIt->vertex_].E = 0;
    }
  }

  FOR_EACH (it, mesh_.faces_) {
    FOR_AROUND_FACE (fIt, (*it)) {

      vertexSubdivision_[fIt->vertex_].sumF += faceVertices_[*it]->point_;
      vertexSubdivision_[fIt->vertex_].sumE += fIt->pair_->vertex_->point_;
      vertexSubdivision_[fIt->vertex_].E++;

      if (fIt->pair_->face_ == 0) {

        if (edgeSubdivision_[fIt].sharpness_ > 0.0) {
          vertexSubdivision_[fIt->pair_->vertex_].sumS += fIt->vertex_->point_;
          vertexSubdivision_[fIt->pair_->vertex_].S++;
          vertexSubdivision_[fIt->pair_->vertex_].smooth_ =
            edgeSubdivision_[fIt].sharpness_;
        }

        vertexSubdivision_[fIt->pair_->vertex_].sumS += fIt->vertex_->point_;
        vertexSubdivision_[fIt->pair_->vertex_].E++;
        vertexSubdivision_[fIt->pair_->vertex_].sumF +=
          fIt->pair_->vertex_->point_;

      }

      if (edgeSubdivision_[fIt].sharpness_ > 0.0) {
        vertexSubdivision_[fIt->vertex_].sumS += fIt->pair_->vertex_->point_;
        vertexSubdivision_[fIt->vertex_].S++;
        vertexSubdivision_[fIt->vertex_].smooth_ =
          edgeSubdivision_[fIt].sharpness_;
      }

    }
  }

  FOR_EACH (it, mesh_.faces_) {
    Vertex<T> *v;
    FOR_AROUND_FACE (fIt, (*it)) {
      v = fIt->vertex_;
      if (vertexSubdivision_[v].nextVertex_ == 0) {

        Vertex<T> *nextV = new Vertex<T>();
        newMesh_->vertices_.push_back(nextV);

        if (vertexSubdivision_[v].S < 2) {
          double E = vertexSubdivision_[v].E;
          nextV->point_ =
            (vertexSubdivision_[v].sumE + vertexSubdivision_[v].sumF)/(E*E) +
            (E - 2.0)/E*v->point_;
        } else if (vertexSubdivision_[v].S == 2) {
          double E = vertexSubdivision_[v].E;
          if (vertexSubdivision_[v].smooth_ >= 1.0) {
            nextV->point_ = ((6.0 * v->point_ +
                              vertexSubdivision_[v].sumS) / 8.0);
          } else {
            double smooth = vertexSubdivision_[v].smooth_;
            nextV->point_ =
              ((6.0 * v->point_ +
                vertexSubdivision_[v].sumS) / 8.0) * smooth +
              ((vertexSubdivision_[v].sumE + vertexSubdivision_[v].sumF)/(E*E) +
               (E - 2.0)/E*v->point_) * (1.0 - smooth);
          }
        } else {
          nextV->point_ = v->point_;
        }
        vertexSubdivision_[v].nextVertex_ = nextV;
      }
    }
  }

}


template <typename T>
void MeshSubdivision<T>::subdivide()
{

  newMesh_ = new Mesh<T>();
  generateFaceVertices();
  generateEdgeVertices();
  generateVertexVertices();

  FOR_EACH (it, mesh_.faces_) {
    FOR_AROUND_FACE (fIt, (*it)) {
      if (edgeSubdivision_[fIt].firstHalf_ == 0) {
        splitHalfEdge(fIt);
        splitHalfEdge(fIt->pair_);
      }
    }
  }

  std::list<Face<T>* > newFaces;

  FOR_EACH (it, mesh_.faces_) {
    FOR_AROUND_FACE (fIt, (*it)) {

      HalfEdge<T> *next = fIt->next_;
      Face<T>     *face = new Face<T>();
      newFaces.push_back(face);

      face->edge_ = edgeSubdivision_[fIt].secondHalf_;

      edgeSubdivision_[fIt].secondHalf_->next_ =
        edgeSubdivision_[next].firstHalf_;

      edgeSubdivision_[next].outGoing_->next_ =
        edgeSubdivision_[fIt].inGoing_;

      edgeSubdivision_[fIt].secondHalf_->pair_ =
        edgeSubdivision_[fIt->pair_].firstHalf_;

      edgeSubdivision_[fIt->pair_].firstHalf_->pair_ =
        edgeSubdivision_[fIt].secondHalf_;

      edgeSubdivision_[next].firstHalf_->pair_ =
        edgeSubdivision_[next->pair_].secondHalf_;
      edgeSubdivision_[next->pair_].secondHalf_->pair_ =
        edgeSubdivision_[next].firstHalf_;

      edgeSubdivision_[fIt].secondHalf_->face_ = face;
      edgeSubdivision_[fIt].inGoing_->face_ = face;
      edgeSubdivision_[next].firstHalf_->face_ = face;
      edgeSubdivision_[next].outGoing_->face_ = face;
    }
  }

  FOR_EACH (it, mesh_.faces_)
    delete (*it);
  mesh_.faces_.clear();

  FOR_EACH (it, mesh_.vertices_)
    delete (*it);
  mesh_.vertices_.clear();

  FOR_EACH (it, mesh_.edges_)
    delete (*it);
  mesh_.edges_.clear();

  FOR_EACH (it, newFaces) {
    mesh_.faces_.push_back(*it);
  }

  FOR_EACH (it, newMesh_->vertices_) {
    mesh_.vertices_.push_back(*it);
  }

  FOR_EACH (it, newMesh_->edges_) {
    mesh_.edges_.push_back(*it);
  }

  delete newMesh_;
}

template <typename T>
bool MeshSubdivision<T>::triangulate()
{
  std::list<Face<T>* > newFaces;
  HalfEdge<T> *e1, *e2;
  Face<T> *f;

  FOR_EACH (it, mesh_.faces_) {

    e1 = new HalfEdge<T>((*it)->edge_->pair_->vertex_);
    e2 = new HalfEdge<T>((*it)->edge_->next_->vertex_);
    mesh_.edges_.push_back(e1);
    mesh_.edges_.push_back(e2);
    pairUp(e1, e2);

    f = new Face<T>(e2);
    e1->face_ = (*it);
    e2->face_ = f;
    newFaces.push_back(f);

    e2->next_ = (*it)->edge_->next_->next_;
    e2->next_->face_ = f;
    e2->next_->next_->face_ = f;
    e2->next_->vertex_->edge_ = e2->next_->next_;
    e2->next_->next_->next_ = e2;

    (*it)->edge_->vertex_->edge_ = (*it)->edge_->next_;
    (*it)->edge_->next_->next_ = e1;
    e1->next_ = (*it)->edge_;
  }

  FOR_EACH (it, newFaces)
    mesh_.faces_.push_back(*it);


  return checkRep();

}


template <typename T>
bool MeshSubdivision<T>::checkRep()
{
  FOR_EACH (it, mesh_.faces_) {
    FOR_AROUND_FACE (fIt, (*it)) {
      if (fIt->pair_->pair_ != fIt) {
        std::cout << "NO SELF LOOP!\n";
        return false;
      }
      if (fIt->face_ != (*it)) {
        std::cout << "NO Face back!\n";
        return false;
      }
      if (fIt->next_->pair_->vertex_ != fIt->vertex_) {
        std::cout << "NO same vertex!!\n";
        return false;
      }
    }
  }
  return true;
}


template <typename T>
void MeshSubdivision<T>::splitHalfEdge(HalfEdge<T>* edge)
{
  if (edge->face_ != 0) {

    HalfEdge<T> *he = new HalfEdge<T>(edgeSubdivision_[edge].edgeVertex_);
    edgeSubdivision_[he].sharpness_ = edgeSubdivision_[edge].sharpness_ - 1.0;
    edgeSubdivision_[edge].firstHalf_ = he;
    newMesh_->edges_.push_back(he);

    he = new HalfEdge<T>(vertexSubdivision_[edge->vertex_].nextVertex_);
    edgeSubdivision_[he].sharpness_ = edgeSubdivision_[edge].sharpness_ - 1.0;
    edgeSubdivision_[edge].secondHalf_ = he;
    newMesh_->edges_.push_back(he);

    he = new HalfEdge<T>(edgeSubdivision_[edge].edgeVertex_);
    edgeSubdivision_[he].sharpness_ = 0;
    edgeSubdivision_[edge].inGoing_ = he;
    newMesh_->edges_.push_back(he);

    he = new HalfEdge<T>(faceVertices_[edge->face_]);
    edgeSubdivision_[he].sharpness_ = 0;
    edgeSubdivision_[edge].outGoing_ = he;
    newMesh_->edges_.push_back(he);

    edgeSubdivision_[edge].firstHalf_->next_ =
      edgeSubdivision_[edge].outGoing_;
    edgeSubdivision_[edge].outGoing_->pair_ =
      edgeSubdivision_[edge].inGoing_;
    edgeSubdivision_[edge].inGoing_->pair_ =
      edgeSubdivision_[edge].outGoing_;
    edgeSubdivision_[edge].inGoing_->next_ =
      edgeSubdivision_[edge].secondHalf_;

  } else {

    HalfEdge<T> *he = new HalfEdge<T>(edge->vertex_);
    edgeSubdivision_[he].sharpness_ = edgeSubdivision_[edge].sharpness_ - 1.0;
    edgeSubdivision_[edge].firstHalf_ = he;
    newMesh_->edges_.push_back(he);

    he = new HalfEdge<T>(vertexSubdivision_[edge->vertex_].nextVertex_);
    edgeSubdivision_[he].sharpness_ = edgeSubdivision_[edge].sharpness_ - 1.0;
    edgeSubdivision_[edge].secondHalf_ = he;
    newMesh_->edges_.push_back(he);


  }

}

}

#endif
