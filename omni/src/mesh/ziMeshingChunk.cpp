#include "project/omProject.h"
#include "volume/omSegmentation.h"
#include "ziMesherManager.h"
#include "ziMeshingChunk.h"
#include "zi/base/base.h"
#include "zi/mesh/HalfEdge.hpp"
#include "zi/mesh/QuadraticErrorSimplification.hpp"
#include "zi/mesh/MarchingCubes.hpp"
#include "zi/vector/Vec.hpp"
#include "zi/mesh/ext/TriStrip/TriStripper.h"
#include "zi/base/time.h"
#include "zi/thread/ThreadFactory.h"

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <cstdlib>
#include <cmath>

using namespace zi::vector;
using boost::shared_ptr;
using zi::Threads::ThreadFactory;
// todo: my debug
#include "common/omDebug.h"

ziMeshingChunk::ziMeshingChunk(int segId, OmMipChunkCoord c,
                               OmMipMeshManager *mmm) :
  segmentationId_(segId),
  mipCoord_(c),
  mipMeshManager_(mmm)
{}

void ziMeshingChunk::run()
{
  uint64_t threadId = ThreadFactory::ID();
  ziTimer ztimer;
  ztimer.start();

  loadChunk_();
  vtkImageData *pImageData = chunk_->GetMeshImageData();
  Vector3<int> dims;
  pImageData->GetDimensions(dims.array);
  OmSegID *pScalarData = static_cast<OmSegID*>(pImageData->GetScalarPointer());

  std::cout << threadId << ": Data Loaded "
            << "(" << ztimer.dLap() << ") :: (" << ztimer.dTotal() << ")\n";


  float maxScale = std::max(std::max(scale_.x(), scale_.y()), scale_.z());
  zi::vector::Vec3d scale = scale_ / maxScale;
  scale_ = zi::vector::Vec3d(maxScale, maxScale, maxScale);

  boost::shared_ptr<boost::unordered_map<int, QuickMesh> > qMeshes =
    zi::QuickMarchingCubes(dims.z-1, dims.y-1, dims.x-1, (int*)pScalarData);

  pImageData->Delete();

  std::cout << threadId << ": Marching Cubes Done "
            << "(" << ztimer.dLap() << ") :: (" << ztimer.dTotal() << ")\n";

  //delete pScalarData;

  double scaleZ = scale.z() * 0.01;
  double scaleY = scale.y() * 0.01;
  double scaleX = scale.x() * 0.01;

  std::map<int, zi::Mesh<Vec3d> > meshes;

  FOR_EACH (it, *qMeshes) {
    meshes[it->first].fromQuickMesh(it->second, scaleZ, scaleY, scaleX);
    meshes[it->first].clearMaps();
    meshes[it->first].calculateVertexNormals();
  }
  qMeshes->clear();

  std::cout << threadId << ": HalfEdge Meshes Created "
            << "(" << ztimer.dLap() << ") :: (" << ztimer.dTotal() << ")\n";


  std::map<int, shared_ptr<zi::QuadraticErrorSimplifier> > simplifiers;
  FOR_EACH (it, meshes) {
    simplifiers[it->first] =
      shared_ptr<zi::QuadraticErrorSimplifier>
      (new zi::QuadraticErrorSimplifier(it->second));
  }

  std::cout << threadId << ": QEM Simplifiers Created "
            << "(" << ztimer.dLap() << ") :: (" << ztimer.dTotal() << ")\n";


  int idx = 0;
  FOR_EACH (it, toDeliver_) {

    std::map<int, std::pair<Tri::tri_stripper::primitives_vector,
      std::vector< zi::Vertex< Vec3d > *> > > stripified;

    FOR_EACH (sit, simplifiers) {
      sit->second->runQueue((idx == 0) ?
                            (meshes[sit->first].faces_.size() / 2) :
                            (meshes[sit->first].faces_.size() / 2),
                            it->second, 0);
      meshes[sit->first].toStrips(&stripified[sit->first].first,
                                  stripified[sit->first].second);
    }

    idx++;
    std::cout << threadId << ": Simplification Round # " << idx << " done "
              << "(" << ztimer.dLap() << ") :: (" << ztimer.dTotal() << ")\n";

    it->first->receive(stripified, scale_, translate_);

    std::cout << threadId << ": Delivered at " << it->second << " "
              << "(" << ztimer.dLap() << ") :: (" << ztimer.dTotal() << ")\n";

    if (it->first->isDone()) {

      std::cout << threadId << ": This Guy is done "
                << "(" << ztimer.dLap() << ") :: (" << ztimer.dTotal() << ")\n";

      shared_ptr<std::map<int, shared_ptr<StrippedMesh> > > sMeshes =
        it->first->getStrippedMeshes();

      //sMeshes
      FOR_EACH (mit, *sMeshes) {
        OmMipMeshCoord mmCoor(it->first->getCoor(), mit->first);
        OmMipMesh *oMesh = mipMeshManager_->AllocMesh(mmCoor);
        oMesh->setSegmentationID(segmentationId_);

        oMesh->mVertexIndexCount = mit->second->totalIndices_;
        oMesh->mpVertexIndexData = new GLuint[oMesh->mVertexIndexCount];
        std::copy(mit->second->indices_.begin(),
                  mit->second->indices_.end(),
                  oMesh->mpVertexIndexData);

        oMesh->mVertexCount = mit->second->totalVertices_;
        oMesh->mpVertexData = new GLfloat[oMesh->mVertexCount * 6];
        std::copy(mit->second->vertices_.begin(),
                  mit->second->vertices_.end(),
                  oMesh->mpVertexData);

        oMesh->mStripCount = mit->second->totalStrips_;
        oMesh->mpStripOffsetSizeData = new uint32_t[2*oMesh->mStripCount];
        std::copy(mit->second->strips_.begin(),
                  mit->second->strips_.end(),
                  oMesh->mpStripOffsetSizeData);

        oMesh->mTrianCount = mit->second->totalTrians_;
        oMesh->mpTrianOffsetSizeData = new uint32_t[2*oMesh->mTrianCount];
        std::copy(mit->second->trians_.begin(),
                  mit->second->trians_.end(),
                  oMesh->mpTrianOffsetSizeData);

        oMesh->Save();
        delete [] oMesh->mpVertexData;
        delete [] oMesh->mpVertexIndexData;
        delete [] oMesh->mpStripOffsetSizeData;
        delete [] oMesh->mpTrianOffsetSizeData;
        delete oMesh;

      }

      std::cout << threadId << ": Meshes Saved "
                << "(" << ztimer.dLap() << ") :: (" << ztimer.dTotal() << ")\n";

    }
  }
  meshes.clear();
  std::cout << threadId << ": ALL DONE !!! "
            << "(" << ztimer.dLap() << ") :: (" << ztimer.dTotal() << ")\n";
}

void ziMeshingChunk::loadChunk_()
{
  debug ("ziMeshingChunk", "loadChunk_()\n");
  OmProject::GetSegmentation(segmentationId_).GetChunk(chunk_, mipCoord_);
  chunk_->Open();
  srcBbox_  = chunk_->GetExtent();
  dstBbox_  = chunk_->GetNormExtent();

  Vector3<float> srcDim = srcBbox_.getMax() - srcBbox_.getMin();
  srcDim += Vector3<float>::ONE;
  Vector3<float> dstDim = dstBbox_.getMax() - dstBbox_.getMin();
  scale_ = zi::vector::Vec3d(dstDim.x / srcDim.x,
                             dstDim.y / srcDim.y,
                             dstDim.z / srcDim.z);

  dstDim = dstBbox_.getMin();
  translate_ = zi::vector::Vec3d(dstDim.z, dstDim.y, dstDim.x);

  cout << "This Chunk has scale: " << scale_
       << ", and translate: " << translate_ << "\n";
  //  cout << "orig dim: " << srcDim.x << ", " << srcDim.y
  //       << ", " << srcDim.z << "\n";
}

void ziMeshingChunk::deliverTo(boost::shared_ptr<GrowingMeshes> level,
                               double maxError)
{
  toDeliver_.push_back(std::make_pair(level, maxError));
}
