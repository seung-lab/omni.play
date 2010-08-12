#include "common/omGl.h"

#include <iostream>
#include "ziMesherManager.h"
#include <zi/mutex>
#include "zi/base/base.h"

using namespace std;
using boost::shared_ptr;

void
StrippedMesh::addStrips(const Tri::tri_stripper::primitives_vector &ptrs,
                        const std::vector< zi::Vertex<zi::vector::Vec3d> *> &vertices,
                        zi::vector::Vec3d scale, zi::vector::Vec3d trans)
{
  {
    zi::Synchronized s(monitor_);
    while (!free_) {
      waiting_++;
      monitor_.wait();
      waiting_--;
    }
  }

  FOR_EACH (it, vertices) {
    zi::vector::Vec3d p = (*it)->point_ * 100.0;
    p *= scale;
    p += trans;
    //std::cout << p << "\n";
    vertices_.push_back((GLfloat)p[2]);
    vertices_.push_back((GLfloat)p[1]);
    vertices_.push_back((GLfloat)p[0]);
    zi::vector::Vec3d n = (*it)->normal_;
    //n.normalize();
    vertices_.push_back((GLfloat)n[2]);
    vertices_.push_back((GLfloat)n[1]);
    vertices_.push_back((GLfloat)n[0]);
  }

  FOR_EACH (it, ptrs) {

    if (it->m_Type == Tri::tri_stripper::PT_Triangles) {
      trians_.push_back(totalIndices_);
      trians_.push_back(it->m_Indices.size());
      ++totalTrians_;
    } else {
      strips_.push_back(totalIndices_);
      strips_.push_back(it->m_Indices.size());
      ++totalStrips_;
    }
    totalIndices_ += it->m_Indices.size();

    FOR_EACH (bit, it->m_Indices) {
      indices_.push_back(totalVertices_ + (*bit));
    }
  }

  totalVertices_ += vertices.size();

  {
    zi::Synchronized s(monitor_);
    if (waiting_)
      monitor_.notify();
  }
}

GrowingMeshes::GrowingMeshes(const OmMipChunkCoord &coor) :
  remainingDeliveries_(1),
  mutex_(),
  coor_(coor)
{
  strippeds_ = shared_ptr<map<int, shared_ptr<StrippedMesh> > >
    (new map<int, shared_ptr<StrippedMesh> > );
}

void GrowingMeshes::receive
(std::map<int, std::pair<Tri::tri_stripper::primitives_vector,
 std::vector< zi::Vertex< zi::vector::Vec3d > *> > > &stripified,
 zi::vector::Vec3d scale, zi::vector::Vec3d trans)
{
  FOR_EACH(it, stripified) {
    boost::shared_ptr<StrippedMesh> p;
    {
      zi::Guard g(mutex_);
      if (strippeds_->find(it->first) == strippeds_->end()) {
        (*strippeds_)[it->first] =
          boost::shared_ptr<StrippedMesh>(new StrippedMesh());
      }
      p = (*strippeds_)[it->first];
    }
    p->addStrips(it->second.first,
                 it->second.second,
                 scale, trans);
  }

  {
    zi::Guard g(mutex_);
    remainingDeliveries_--;
  }

}

