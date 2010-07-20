/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#include "QuadraticErrorSimplification.hpp"

#include "zi/base/base.h"
#include <map>
#include <iostream>

namespace zi {

#define QES_BAD_ANGLE 0.2

using namespace std;
using namespace zi::vector;

QuadraticErrorSimplifier::QuadraticErrorSimplifier(Mesh<vector::Vec3d> &mesh)
  : mesh_(mesh)
{
  generateQValues();
}

QuadraticErrorSimplifier::~QuadraticErrorSimplifier()
{
  std::cout << "KILLING QUEUE\n";
  queue_.clear();
  QValues_.clear();
}

void QuadraticErrorSimplifier::generateQValues()
{
  QValues_.clear();
  FOR_EACH (it, mesh_.vertices_) {
    if ((*it)->border_)
      continue;
    Mat4d Q = Mat4d::ZERO();
    FOR_AROUND_VERTEX(fedge, (*it)) {
      Vec4d arow(fedge->face_->getNormal(),
                 -fedge->face_->getNormal().dot((*it)->point_));
      Mat4d m(arow);
      m[0] *= arow[0];
      m[1] *= arow[1];
      m[2] *= arow[2];
      m[3] *= arow[3];
      m *= fedge->face_->realNormal().len();
      Q += m;
    }
    QValues_[*it] = Q;
  }
}

void QuadraticErrorSimplifier::queueInit()
{
  //queue_.clear();
  //errValues_.clear();
  FOR_EACH (it, mesh_.edges_) {
    if ((*it)->vertex_) {
      addToQueue(*it);
    }
  }
}

bool QuadraticErrorSimplifier::addToQueue(HalfEdge<Vec3d>* edge)
{

  assert(edge->pair_ != edge);
  //assert(errValues_.find(edge) == errValues_.end());

  if (edge->vertex_->border_ || edge->pair_->vertex_->border_)
    return false;
  if (edge->next_->pair_->face_ == edge->next_->next_->pair_->face_)
    return false;
  if (edge->pair_->next_->pair_->face_ == edge->pair_->next_->next_->pair_->face_)
    return false;

  if (edge->next_->pair_->face_ == edge->pair_->next_->pair_->face_)
    return false;
  if (edge->next_->next_->pair_->face_ == edge->pair_->next_->next_->pair_->face_)
    return false;

  if (edge->next_->pair_->face_ == edge->next_->next_->pair_->face_)
    return false;
  if (edge->pair_->next_->pair_->face_ == edge->pair_->next_->next_->pair_->face_)
    return false;


  if (edge->next_->pair_->face_ == edge->pair_->face_)
    return false;
  if (edge->next_->pair_->face_ == edge->face_)
    return false;
  if (edge->next_->next_->pair_->face_ == edge->pair_->face_)
    return false;
  if (edge->next_->next_->pair_->face_ == edge->face_)
    return false;
  if (edge->pair_->next_->pair_->face_ == edge->pair_->face_)
    return false;
  if (edge->pair_->next_->pair_->face_ == edge->face_)
    return false;
  if (edge->pair_->next_->next_->pair_->face_ == edge->pair_->face_)
    return false;
  if (edge->pair_->next_->next_->pair_->face_ == edge->face_)
    return false;

  if ((edge->next_->vertex_ == edge->pair_->vertex_) ||
      (edge->next_->vertex_ == edge->pair_->next_->vertex_) ||
      (edge->vertex_ == edge->pair_->next_->vertex_)) {
    //std::cout << "ZAJKEBASAD\n";
    return false;
  }

  int cnt = 0;
  FOR_AROUND_VERTEX(ii, edge->vertex_) {
    if (ii != edge)
      if (ii->vertex_ == edge->vertex_) {
        //std::cout << "BAD CHOICE\n";
        return false;
      }
    cnt++;
  }

  if (cnt < 4)
    return false;

  cnt = 0;
  FOR_AROUND_VERTEX(ii, edge->pair_->vertex_) {
    cnt++;
    if (ii != edge->pair_)
      if (ii->vertex_ == edge->pair_->vertex_) {
        //std::cout << "BAD CHOICE2\n";
        return false;
      }
  }

  if (cnt < 4)
    return false;


  Vertex<Vec3d> *vx1 = edge->pair_->vertex_;
  Vertex<Vec3d> *vx2 = edge->vertex_;

  double err;
  Vec4d VBest;

  Mat4d Q = QValues_[vx1] + QValues_[vx2];
  Mat4d QOrig = Q;
  Q[3] = Vec4d(0,0,0,1);

  double d = det(Q);
  if ((d < 0.000001) && (d > -0.000001)) {
    Vec4d v1(vx1->point_, 1);
    Vec4d v2(vx2->point_, 1);
    double qt = ((v2-v1)*Q).dot(v2-v1);
    double rs = (v1*Q).dot(v2-v1);
    if ((qt < 0.000001f) && (qt > -0.000001f)) {
      VBest = (v1 +v2)/2.0;
    } else {
      VBest = v1 - rs*(v2-v1)/qt;
    }
  } else {
    VBest = Vec4d(0,0,0,1) * inv(Q);
  }

  err = (VBest * QOrig).dot(VBest);

  edge->deleteAttachment();
  edge->attachment_ = new Vec3d(VBest);

  //queue_[make_pair(err, edge)] = VBest;
  //errValues_[edge] = err;
  queue_.push(err, *edge);

  return true;

}

int QuadraticErrorSimplifier::popQueue()
{

  if (queue_.size() == 0) {
    assert(false);
    return 0;
  }

  //std::cout << "QILL\n";
  //  std::cout.flush();

  HalfEdge<Vec3d>* edge = queue_.topNode<HalfEdge<Vec3d> >();
  Vec3d newVx1(*edge->attachment_);

  //  std::cout << "LALA: " << newVx1 << "\n";
  //  std::cout.flush();

  queue_.erase(*edge);
  edge->deleteAttachment();

  if (edge->next_->pair_->face_ == edge->next_->next_->pair_->face_)
    return 0;
  if (edge->pair_->next_->pair_->face_ == edge->pair_->next_->next_->pair_->face_)
    return 0;

  if (edge->next_->pair_->face_ == edge->pair_->next_->pair_->face_)
    return 0;
  if (edge->next_->next_->pair_->face_ == edge->pair_->next_->next_->pair_->face_)
    return 0;

  if (edge->next_->pair_->face_ == edge->next_->next_->pair_->face_)
    return 0;
  if (edge->pair_->next_->pair_->face_ == edge->pair_->next_->next_->pair_->face_)
    return 0;

  if ((edge->next_->vertex_ == edge->pair_->vertex_) ||
      (edge->next_->vertex_ == edge->pair_->next_->vertex_) ||
      (edge->vertex_ == edge->pair_->next_->vertex_)) {
    //cout << "ZAJKEBASAD\n";
    return 0;
  }

  zi::Vertex<Vec3d> *vx2 = edge->vertex_;
  zi::Vertex<Vec3d> *vx1 = edge->pair_->vertex_;

  if (vx1 == vx2) {
    cout << "FORM: " << endl;
    cout << vx1->point_ << " AND "
         << vx2->point_ << " ---> " << newVx1 << std::endl;
    assert(false);
  }

  FOR_AROUND_VERTEX(ii, vx1) {
    if (ii != edge)
      if (ii->vertex_ == vx2) {
        //cout << "===> BAD CHOICE\n";
        return 0;
      }
  }

  FOR_AROUND_VERTEX(ii, vx2) {
    if (ii != edge->pair_)
      if (ii->vertex_ == vx1) {
        //cout << "===> BAD CHOICE\n";
        return 0;
      }
  }


  FOR_AROUND_VERTEX (it, vx2) {
    if ((it != edge->next_) && (it != edge->pair_)) {
      Vec3d newA = zi::vector::cross(it->vertex_->point_ - newVx1,
                                     it->next_->vertex_->point_ - newVx1);

      if (it->face_->getNormal().dot(newA.norm()) < QES_BAD_ANGLE)  {
        return 0;
      }
    }
  }

  FOR_AROUND_VERTEX (it, vx1) {
    if ((it != edge->pair_->next_) && (it != edge)) {
      Vec3d newA = zi::vector::cross(it->vertex_->point_ - newVx1,
                                     it->next_->vertex_->point_ - newVx1);
      if (it->face_->getNormal().dot(newA.norm()) < QES_BAD_ANGLE) {
        return 0;
      }
    }
  }

  int nDiff = -1;

  FOR_AROUND_VERTEX (e, vx1) {
    if (e->attachment_ != 0) {
      --nDiff;
      queue_.erase(*e);
      e->deleteAttachment();
    }
    if (e->pair_->attachment_ != 0) {
      --nDiff;
      queue_.erase(*e->pair_);
      e->pair_->deleteAttachment();
    }
  }

  FOR_AROUND_VERTEX (e, vx2) {
    if (e->attachment_ != 0) {
      --nDiff;
      queue_.erase(*e);
      e->deleteAttachment();
    }
    if (e->pair_->attachment_ != 0) {
      --nDiff;
      queue_.erase(*e->pair_);
      e->pair_->deleteAttachment();
    }
  }

  vx1->point_ = newVx1;
  vx1->normal_ += vx2->normal_;
  //vx1->normal_.normalize();

  QValues_[vx1] += QValues_[vx2];
  QValues_.erase(vx2);

  collapseEdge(edge);

  FOR_AROUND_VERTEX (e, vx1) {
    //assert(e->vertex_ != vx1);
    if (addToQueue(e)) nDiff++;
  }

  return -2;

}

void QuadraticErrorSimplifier::collectMeshGarbage()
{

  ITERATOR(mesh_.faces_) it = mesh_.faces_.begin();
  while (it != mesh_.faces_.end()) {
    if ((*it)->edge_ == 0) {
      //delete (*it);
      it = mesh_.faces_.erase(it);
    } else {
      it++;
    }
  }

  ITERATOR(mesh_.vertices_) itv = mesh_.vertices_.begin();
  while (itv != mesh_.vertices_.end()) {
    if ((*itv)->edge_ == 0) {
      //delete (*itv);
      itv = mesh_.vertices_.erase(itv);
    } else {
      itv++;
    }
  }

  ITERATOR(mesh_.edges_) ite = mesh_.edges_.begin();
  while (ite != mesh_.edges_.end()) {
    if ((*ite)->vertex_ == 0) {
      //delete (*ite);
      ite = mesh_.edges_.erase(ite);
    } else {
      ite++;
    }
  }


  mesh_.checkRep();

}

int QuadraticErrorSimplifier::runQueue(int triangleNoLimit,
                                       double errorLimit,
                                       int queueSizeLimit)
{
  queueInit();
  std::cout << "QUEUE INITIALIZED: " << queue_.size() << "\n";
  int res = mesh_.faces_.size();
  while ((res > triangleNoLimit) &&
         (queue_.size() > queueSizeLimit) &&
         (queue_.size() > 0) &&
         (queue_.top() < errorLimit)) {
    res += popQueue();
    //std::cout << "  EDGE SIZE: " << errValues_.size() << "\n";
    //std::cout << "  NEXT ERR: " << queue_.begin()->first.first << "\n";
  }
  std::cout << "QUEUE DONE: " << queue_.size() << "\n";
  std::cout << "   NO TRIANGLES: " << res << "\n";
  if (queue_.size() > 0) {
    std::cout << "   NEXT ERR: " << queue_.top() << "\n";
  }
  collectMeshGarbage();
  return res;
}

#undef QES_BAD_ANGLE

}
